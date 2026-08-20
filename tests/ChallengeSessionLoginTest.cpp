#include <cassert>
#include <map>
#include <string>

#include "ChallengeSessionLogin.h"

namespace challenge_session_login_tests {
namespace {

class FixedWallClock : public server::WallClock {
public:
    explicit FixedWallClock(int64_t now) : nowUnix(now) {}
    int64_t nowUnixSeconds() const { return nowUnix; }
    int64_t nowUnix;
};

class FakeChallengeClaimPort : public server::PlayerChallengeClaimPort {
public:
    struct Row {
        int64_t userId;
        int64_t expiresAtUnix;
        bool claimed;
    };

    std::map<std::string, Row> rows;
    int claimAttempts;

    FakeChallengeClaimPort() : claimAttempts(0) {}

    bool claim(const server::ChallengeKey& key,
               int64_t nowUnixSeconds,
               int64_t& outUserId) {
        ++claimAttempts;
        std::map<std::string, Row>::iterator it = rows.find(key.value);
        if (it == rows.end()) {
            return false;
        }
        if (it->second.claimed || it->second.expiresAtUnix <= nowUnixSeconds) {
            return false;
        }
        it->second.claimed = true;
        outUserId = it->second.userId;
        return true;
    }
};

class FakeSessionStore : public server::PlayerSessionStorePort {
public:
    struct Row {
        int64_t userId;
        int64_t expiresAtUnix;
    };

    std::map<std::string, Row> rows;
    int createCount;

    FakeSessionStore() : createCount(0) {}

    void create(int64_t userId,
                const server::PlayerSessionKey& key,
                int64_t expiresAtUnix) {
        ++createCount;
        Row row;
        row.userId = userId;
        row.expiresAtUnix = expiresAtUnix;
        rows[key.value] = row;
    }

    bool extend(const server::PlayerSessionKey& key,
                int64_t nowUnixSeconds,
                int64_t newExpiresAtUnix) {
        std::map<std::string, Row>::iterator it = rows.find(key.value);
        if (it == rows.end() || it->second.expiresAtUnix <= nowUnixSeconds) {
            return false;
        }
        it->second.expiresAtUnix = newExpiresAtUnix;
        return true;
    }

    bool isActive(const server::PlayerSessionKey& key,
                  int64_t nowUnixSeconds) const {
        std::map<std::string, Row>::const_iterator it = rows.find(key.value);
        if (it == rows.end()) {
            return false;
        }
        return it->second.expiresAtUnix > nowUnixSeconds;
    }
};

class FixedKeyIssuer : public server::SessionKeyIssuer {
public:
    explicit FixedKeyIssuer(const std::string& value) : nextValue(value) {}

    server::PlayerSessionKey issue() {
        server::PlayerSessionKey key;
        key.value = nextValue;
        return key;
    }

    std::string nextValue;
};

} // namespace

void claims_valid_challenge_and_issues_thirty_minute_session() {
    const int64_t now = 1'700'000'000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeChallengeClaimPort::Row challenge;
    challenge.userId = 42;
    challenge.expiresAtUnix = now + 120;
    challenge.claimed = false;
    challenges.rows["challenge-ok"] = challenge;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("session-1");
    server::ChallengeSessionLoginService service(challenges, sessions, keys, clock);

    const server::ChallengeLoginResult result =
        service.loginWithChallenge(server::ChallengeKey{"challenge-ok"});

    assert(result.accepted);
    assert(result.userId == 42);
    assert(result.sessionKey.value == "session-1");
    assert(result.expiresAtUnix == now + server::ChallengeSessionLoginService::kSessionTtlSeconds);
    assert(sessions.createCount == 1);
    assert(challenges.rows["challenge-ok"].claimed);
}

void rejects_expired_challenge_without_creating_a_session() {
    const int64_t now = 1'700'000'000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeChallengeClaimPort::Row challenge;
    challenge.userId = 7;
    challenge.expiresAtUnix = now; // not strictly after now
    challenge.claimed = false;
    challenges.rows["challenge-expired"] = challenge;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("session-x");
    server::ChallengeSessionLoginService service(challenges, sessions, keys, clock);

    const server::ChallengeLoginResult result =
        service.loginWithChallenge(server::ChallengeKey{"challenge-expired"});

    assert(!result.accepted);
    assert(result.reason == "invalid_challenge");
    assert(sessions.createCount == 0);
}

void rejects_already_claimed_challenge() {
    const int64_t now = 1'700'000'000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeChallengeClaimPort::Row challenge;
    challenge.userId = 7;
    challenge.expiresAtUnix = now + 120;
    challenge.claimed = true;
    challenges.rows["challenge-used"] = challenge;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("session-x");
    server::ChallengeSessionLoginService service(challenges, sessions, keys, clock);

    const server::ChallengeLoginResult result =
        service.loginWithChallenge(server::ChallengeKey{"challenge-used"});

    assert(!result.accepted);
    assert(result.reason == "invalid_challenge");
    assert(sessions.createCount == 0);
}

void keep_alive_extends_active_session_by_thirty_minutes() {
    const int64_t now = 1'700'000'000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FakeSessionStore::Row session;
    session.userId = 42;
    session.expiresAtUnix = now + 60;
    sessions.rows["session-live"] = session;
    FixedKeyIssuer keys("unused");
    server::ChallengeSessionLoginService service(challenges, sessions, keys, clock);

    const server::KeepAliveResult result =
        service.keepAlive(server::PlayerSessionKey{"session-live"});

    assert(result.accepted);
    assert(result.expiresAtUnix == now + server::ChallengeSessionLoginService::kSessionTtlSeconds);
    assert(sessions.rows["session-live"].expiresAtUnix == result.expiresAtUnix);
}

void validate_session_accepts_unexpired_key_for_reconnect() {
    const int64_t now = 1'700'000'000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FakeSessionStore::Row session;
    session.userId = 42;
    session.expiresAtUnix = now + 10;
    sessions.rows["session-live"] = session;
    FixedKeyIssuer keys("unused");
    server::ChallengeSessionLoginService service(challenges, sessions, keys, clock);

    assert(service.validateSession(server::PlayerSessionKey{"session-live"}));
    clock.nowUnix = now + 10;
    assert(!service.validateSession(server::PlayerSessionKey{"session-live"}));
}

} // namespace challenge_session_login_tests

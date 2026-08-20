#include <cassert>
#include <map>
#include <string>

#include "ChallengeLoginCommandHandler.h"
#include "ChallengeSessionLogin.h"
#include "NetworkCommand.h"

namespace challenge_login_command_handler_tests {
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

    bool claim(const server::ChallengeKey& key,
               int64_t nowUnixSeconds,
               int64_t& outUserId) {
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

    void create(int64_t userId,
                const server::PlayerSessionKey& key,
                int64_t expiresAtUnix) {
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
        return it != rows.end() && it->second.expiresAtUnix > nowUnixSeconds;
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

class FakeGameplaySessionPort : public server::GameplaySessionPort {
public:
    int openCount;
    int64_t lastUserId;

    FakeGameplaySessionPort() : openCount(0), lastUserId(0) {}

    session::SessionInfo openAuthenticated(int64_t userId) {
        ++openCount;
        lastUserId = userId;
        session::SessionInfo info;
        info.internalId = 100 + openCount;
        info.aliasId = 0;
        info.claimedId.clear();
        info.authenticated = true;
        return info;
    }
};

server::ChallengeSessionLoginService makeAuth(FakeChallengeClaimPort& challenges,
                                              FakeSessionStore& sessions,
                                              FixedKeyIssuer& keys,
                                              FixedWallClock& clock) {
    return server::ChallengeSessionLoginService(challenges, sessions, keys, clock);
}

} // namespace

void accepts_login_when_payload_is_a_valid_challenge_key() {
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeChallengeClaimPort::Row row;
    row.userId = 42;
    row.expiresAtUnix = now + 120;
    row.claimed = false;
    challenges.rows["challenge-ok"] = row;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("player-session-1");
    server::ChallengeSessionLoginService auth =
        makeAuth(challenges, sessions, keys, clock);
    FakeGameplaySessionPort gameplay;
    server::ChallengeLoginCommandHandler handler(auth, gameplay);

    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "challenge-ok"
    };

    const server::ChallengeLoginCommandResult result = handler.handle(command);

    assert(result.accepted);
    assert(result.error.empty());
    assert(result.userId == 42);
    assert(result.playerSessionKey.value == "player-session-1");
    assert(result.session.authenticated);
    assert(result.session.internalId > 0);
    assert(gameplay.openCount == 1);
    assert(gameplay.lastUserId == 42);
    assert(challenges.rows["challenge-ok"].claimed);
    assert(sessions.rows.count("player-session-1") == 1);
}

void rejects_login_when_challenge_is_invalid() {
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("unused");
    server::ChallengeSessionLoginService auth =
        makeAuth(challenges, sessions, keys, clock);
    FakeGameplaySessionPort gameplay;
    server::ChallengeLoginCommandHandler handler(auth, gameplay);

    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "missing-challenge"
    };

    const server::ChallengeLoginCommandResult result = handler.handle(command);

    assert(!result.accepted);
    assert(result.error == "invalid_challenge");
    assert(result.session.internalId == 0);
    assert(!result.session.authenticated);
    assert(gameplay.openCount == 0);
    assert(sessions.rows.empty());
}

void rejects_client_supplied_internal_id_on_challenge_login() {
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeChallengeClaimPort::Row row;
    row.userId = 42;
    row.expiresAtUnix = now + 120;
    row.claimed = false;
    challenges.rows["challenge-ok"] = row;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("player-session-1");
    server::ChallengeSessionLoginService auth =
        makeAuth(challenges, sessions, keys, clock);
    FakeGameplaySessionPort gameplay;
    server::ChallengeLoginCommandHandler handler(auth, gameplay);

    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        42,
        "challenge-ok"
    };

    const server::ChallengeLoginCommandResult result = handler.handle(command);

    assert(!result.accepted);
    assert(result.session.internalId == 0);
    assert(gameplay.openCount == 0);
    assert(!challenges.rows["challenge-ok"].claimed);
}

} // namespace challenge_login_command_handler_tests

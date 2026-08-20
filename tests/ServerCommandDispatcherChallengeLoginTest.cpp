#include <cassert>
#include <map>
#include <string>

#include "ChallengeSessionLogin.h"
#include "GameplaySessionPort.h"
#include "ServerCommandDispatcher.h"

namespace server_command_dispatcher_challenge_login_tests {
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
        if (it == rows.end() || it->second.claimed ||
            it->second.expiresAtUnix <= nowUnixSeconds) {
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

    FakeGameplaySessionPort() : openCount(0) {}

    session::SessionInfo openAuthenticated(int64_t userId) {
        ++openCount;
        session::SessionInfo info;
        info.internalId = 200 + openCount;
        info.aliasId = 0;
        info.claimedId.clear();
        info.authenticated = true;
        (void)userId;
        return info;
    }
};

} // namespace

void dispatches_login_with_valid_challenge_key() {
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
    server::ChallengeSessionLoginService auth(challenges, sessions, keys, clock);
    FakeGameplaySessionPort gameplay;
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry, auth, gameplay);

    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        "challenge-ok"};
    const server::CommandDispatchResult result = dispatcher.dispatch(command);

    assert(result.accepted);
    assert(result.session.authenticated);
    assert(result.session.internalId > 0);
    assert(result.playerSessionKey.value == "player-session-1");
    assert(result.error.empty());
    assert(challenges.rows["challenge-ok"].claimed);
}

void rejects_nickname_login_when_challenge_auth_is_bound() {
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("unused");
    server::ChallengeSessionLoginService auth(challenges, sessions, keys, clock);
    FakeGameplaySessionPort gameplay;
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry, auth, gameplay);

    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        "player"};
    const server::CommandDispatchResult result = dispatcher.dispatch(command);

    assert(!result.accepted);
    assert(result.error == "invalid_challenge");
    assert(result.session.internalId == 0);
    assert(gameplay.openCount == 0);
}

} // namespace server_command_dispatcher_challenge_login_tests

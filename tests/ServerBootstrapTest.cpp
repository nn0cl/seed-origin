#include <cassert>
#include <cstdlib>
#include <map>
#include <string>

#include "ChallengeSessionLogin.h"
#include "GameplaySessionPort.h"
#include "ServerBootstrap.h"

namespace server_bootstrap_tests {
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
    session::SessionInfo openAuthenticated(int64_t userId) {
        session::SessionInfo info;
        info.internalId = 200;
        info.aliasId = 0;
        info.claimedId.clear();
        info.authenticated = true;
        (void)userId;
        return info;
    }
};

void restoreChallengeAuthEnv(const char* saved) {
    if (saved == nullptr) {
        unsetenv("SEED_CHALLENGE_AUTH");
        return;
    }
    setenv("SEED_CHALLENGE_AUTH", saved, 1);
}

} // namespace

void create_dispatcher_uses_challenge_login_when_bundle_provided() {
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("player-session-1");
    server::ChallengeSessionLoginService auth(challenges, sessions, keys, clock);
    FakeGameplaySessionPort gameplay;
    server::ChallengeAuthBundle bundle = {auth, gameplay};
    session::SessionRegistry registry;

    const std::unique_ptr<server::ServerCommandDispatcher> dispatcher =
        server::ServerBootstrap::createCommandDispatcher(registry, &bundle);
    assert(dispatcher.get() != nullptr);

    const network::NetworkCommand nicknameLogin = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        "player"};
    const server::CommandDispatchResult nicknameResult =
        dispatcher->dispatch(nicknameLogin);
    assert(!nicknameResult.accepted);
    assert(nicknameResult.error == "invalid_challenge");

    FakeChallengeClaimPort::Row row;
    row.userId = 42;
    row.expiresAtUnix = now + 120;
    row.claimed = false;
    challenges.rows["challenge-ok"] = row;

    const network::NetworkCommand challengeLogin = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        "challenge-ok"};
    const server::CommandDispatchResult challengeResult =
        dispatcher->dispatch(challengeLogin);
    assert(challengeResult.accepted);
    assert(challengeResult.playerSessionKey.value == "player-session-1");
}

void create_dispatcher_uses_anonymous_login_when_bundle_is_null() {
    session::SessionRegistry registry;
    const std::unique_ptr<server::ServerCommandDispatcher> dispatcher =
        server::ServerBootstrap::createCommandDispatcher(registry, nullptr);
    assert(dispatcher.get() != nullptr);

    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        "player"};
    const server::CommandDispatchResult result = dispatcher->dispatch(command);
    assert(result.accepted);
    assert(result.session.internalId != 0);
    assert(!result.session.authenticated);
}

void production_dispatcher_uses_anonymous_path_when_env_is_unset() {
    const char* saved = std::getenv("SEED_CHALLENGE_AUTH");
    unsetenv("SEED_CHALLENGE_AUTH");

    session::SessionRegistry registry;
    std::string error;
    const std::unique_ptr<server::ServerCommandDispatcher> dispatcher =
        server::ServerBootstrap::createProductionCommandDispatcher(registry,
                                                                   error);
    assert(dispatcher.get() != nullptr);
    assert(error.empty());

    restoreChallengeAuthEnv(saved);
}

void production_dispatcher_fails_when_challenge_auth_env_is_set() {
    const char* saved = std::getenv("SEED_CHALLENGE_AUTH");
    setenv("SEED_CHALLENGE_AUTH", "1", 1);

    session::SessionRegistry registry;
    std::string error;
    const std::unique_ptr<server::ServerCommandDispatcher> dispatcher =
        server::ServerBootstrap::createProductionCommandDispatcher(registry,
                                                                   error);
    assert(dispatcher.get() == nullptr);
    assert(!error.empty());

    restoreChallengeAuthEnv(saved);
}

void challenge_auth_enabled_from_environment() {
    const char* saved = std::getenv("SEED_CHALLENGE_AUTH");

    setenv("SEED_CHALLENGE_AUTH", "1", 1);
    assert(server::ServerBootstrap::challengeAuthEnabledFromEnvironment());

    setenv("SEED_CHALLENGE_AUTH", "true", 1);
    assert(server::ServerBootstrap::challengeAuthEnabledFromEnvironment());

    setenv("SEED_CHALLENGE_AUTH", "0", 1);
    assert(!server::ServerBootstrap::challengeAuthEnabledFromEnvironment());

    unsetenv("SEED_CHALLENGE_AUTH");
    assert(!server::ServerBootstrap::challengeAuthEnabledFromEnvironment());

    restoreChallengeAuthEnv(saved);
}

} // namespace server_bootstrap_tests

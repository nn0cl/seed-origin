#ifndef SEED_SERVER_BOOTSTRAP_H
#define SEED_SERVER_BOOTSTRAP_H

#include <memory>
#include <string>

#include "ChallengeSessionLogin.h"
#include "GameplaySessionPort.h"
#include "ServerCommandDispatcher.h"

namespace server {

// Challenge-auth wiring for the seed_server composition root (LISS-0147).
struct ChallengeAuthBundle {
    ChallengeSessionLoginService& auth;
    GameplaySessionPort& gameplay;
};

class ServerBootstrap {
public:
    // Returns a dispatcher on the anonymous login path when challengeAuth is
    // nullptr; otherwise binds challenge Login via ChallengeAuthBundle.
    static std::unique_ptr<ServerCommandDispatcher> createCommandDispatcher(
        session::SessionRegistry& registry,
        ChallengeAuthBundle* challengeAuth = nullptr);

    // True when SEED_CHALLENGE_AUTH is set to a truthy value ("1" or "true").
    static bool challengeAuthEnabledFromEnvironment();

    // Composition-root wiring for seed_server. Returns nullptr and sets error
    // when challenge auth is enabled without Postgres challenge adapters.
    static std::unique_ptr<ServerCommandDispatcher> createProductionCommandDispatcher(
        session::SessionRegistry& registry,
        std::string& error);
};

}

#endif

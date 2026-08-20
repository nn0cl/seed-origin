#ifndef SEED_POSTGRES_CHALLENGE_SERVER_BOOTSTRAP_H
#define SEED_POSTGRES_CHALLENGE_SERVER_BOOTSTRAP_H

#include <memory>
#include <string>

#include "ChallengeSessionLogin.h"
#include "PostgresPlayerSessionStore.h"
#include "RandomSessionKeyIssuer.h"
#include "RegistryGameplaySessionPort.h"
#include "ServerBootstrap.h"
#include "SystemWallClock.h"

namespace server {

// Owns live Postgres challenge-auth wiring for seed_server (LISS-0147).
struct PostgresChallengeProductionState {
    std::unique_ptr<PostgresPlayerSessionStore> sessionStore;
    std::unique_ptr<SystemWallClock> clock;
    std::unique_ptr<RandomSessionKeyIssuer> keyIssuer;
    std::unique_ptr<RegistryGameplaySessionPort> gameplay;
    std::unique_ptr<ChallengeSessionLoginService> authService;
};

// Builds a challenge-login dispatcher when SEED_CHALLENGE_AUTH and
// SEED_IDENTITY_DB_URL are available. Returns nullptr and sets error otherwise.
std::unique_ptr<ServerCommandDispatcher> createPostgresChallengeProductionDispatcher(
    session::SessionRegistry& registry,
    PostgresChallengeProductionState& ownedState,
    std::string& error);

}

#endif

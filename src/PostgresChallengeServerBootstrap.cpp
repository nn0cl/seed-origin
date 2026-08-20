#include "PostgresChallengeServerBootstrap.h"

#include "ChallengeProductionBootstrapErrors.h"
#include "PostgresProductionBootstrapBridge.h"

namespace server {

namespace {

PostgresChallengeProductionState* resolveOwnedState(void* ownedState,
                                                    PostgresChallengeProductionState& localState) {
    return ownedState != nullptr
        ? static_cast<PostgresChallengeProductionState*>(ownedState)
        : &localState;
}

bool initializeOwnedState(session::SessionRegistry& registry,
                          PostgresChallengeProductionState& ownedState) {
    ownedState.sessionStore = PostgresPlayerSessionStore::fromEnvironment();
    if (!ownedState.sessionStore) {
        return false;
    }

    ownedState.clock = std::make_unique<SystemWallClock>();
    ownedState.keyIssuer = std::make_unique<RandomSessionKeyIssuer>();
    ownedState.gameplay = std::make_unique<RegistryGameplaySessionPort>(registry);
    ownedState.authService = std::make_unique<ChallengeSessionLoginService>(
        *ownedState.sessionStore, *ownedState.sessionStore, *ownedState.keyIssuer,
        *ownedState.clock);
    return true;
}

}

std::unique_ptr<ServerCommandDispatcher> createPostgresChallengeProductionDispatcher(
    session::SessionRegistry& registry,
    PostgresChallengeProductionState& ownedState,
    std::string& error) {
    error.clear();

    if (!initializeOwnedState(registry, ownedState)) {
        error = kMissingIdentityDbUrlError;
        return nullptr;
    }

    ChallengeAuthBundle bundle = {*ownedState.authService, *ownedState.gameplay};
    return ServerBootstrap::createCommandDispatcher(registry, &bundle);
}

std::unique_ptr<ServerCommandDispatcher> postgresProductionBootstrap(
    session::SessionRegistry& registry,
    std::string& error,
    void* ownedState) {
    PostgresChallengeProductionState localState;
    return createPostgresChallengeProductionDispatcher(
        registry, *resolveOwnedState(ownedState, localState), error);
}

}

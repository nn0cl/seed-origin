#ifndef SEED_REGISTRY_GAMEPLAY_SESSION_PORT_H
#define SEED_REGISTRY_GAMEPLAY_SESSION_PORT_H

#include "GameplaySessionPort.h"
#include "SessionRegistry.h"

namespace server {

// Opens an authenticated in-world session after ChallengeKey claim, without
// using anonymous nickname login.
class RegistryGameplaySessionPort : public GameplaySessionPort {
public:
    explicit RegistryGameplaySessionPort(session::SessionRegistry& registry);

    session::SessionInfo openAuthenticated(int64_t userId) override;

private:
    session::SessionRegistry& registry;
};

}

#endif

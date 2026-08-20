#ifndef SEED_GAMEPLAY_SESSION_PORT_H
#define SEED_GAMEPLAY_SESSION_PORT_H

#include <cstdint>

#include "SessionRegistry.h"

namespace server {

// Allocates an in-world session after a ChallengeKey has already been claimed.
// Keeps SessionRegistry / alias rules out of the challenge auth use-case.
class GameplaySessionPort {
public:
    virtual ~GameplaySessionPort() {}
    virtual session::SessionInfo openAuthenticated(int64_t userId) = 0;
};

}

#endif

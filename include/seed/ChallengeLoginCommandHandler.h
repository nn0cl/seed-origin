#ifndef SEED_CHALLENGE_LOGIN_COMMAND_HANDLER_H
#define SEED_CHALLENGE_LOGIN_COMMAND_HANDLER_H

#include <cstdint>
#include <string>

#include "ChallengeSessionLogin.h"
#include "NetworkCommand.h"
#include "SessionRegistry.h"

namespace server {

// LISS-0147 wire slice: Login Command payload is a ChallengeKey.
// Gameplay session allocation stays behind a port so SessionRegistry can be
// adapted without the use-case depending on alias/claimed-id rules.

class GameplaySessionPort {
public:
    virtual ~GameplaySessionPort() {}
    virtual session::SessionInfo openAuthenticated(int64_t userId) = 0;
};

struct ChallengeLoginCommandResult {
    bool accepted;
    std::string error;
    session::SessionInfo session;
    PlayerSessionKey playerSessionKey;
    int64_t userId;
};

class ChallengeLoginCommandHandler {
public:
    ChallengeLoginCommandHandler(ChallengeSessionLoginService& auth,
                                 GameplaySessionPort& gameplaySessions);

    ChallengeLoginCommandResult handle(const network::NetworkCommand& command);

private:
    ChallengeSessionLoginService& auth;
    GameplaySessionPort& gameplaySessions;
};

}

#endif

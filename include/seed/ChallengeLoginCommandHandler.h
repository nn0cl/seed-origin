#ifndef SEED_CHALLENGE_LOGIN_COMMAND_HANDLER_H
#define SEED_CHALLENGE_LOGIN_COMMAND_HANDLER_H

#include <cstdint>
#include <string>

#include "ChallengeSessionLogin.h"
#include "GameplaySessionPort.h"
#include "NetworkCommand.h"
#include "SessionRegistry.h"

namespace server {

// LISS-0147 wire slice: Login Command payload is a ChallengeKey.

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
    static ChallengeLoginCommandResult rejected(const std::string& error);
    static ChallengeLoginCommandResult accepted(int64_t userId,
                                                const PlayerSessionKey& sessionKey,
                                                const session::SessionInfo& session);

    ChallengeSessionLoginService& auth;
    GameplaySessionPort& gameplaySessions;
};

}

#endif

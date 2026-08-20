#ifndef SEED_SERVER_COMMAND_DISPATCHER_H
#define SEED_SERVER_COMMAND_DISPATCHER_H

#include <cstddef>
#include <string>
#include <vector>

#include "LoginCommandHandler.h"
#include "ChallengeLoginCommandHandler.h"
#include "CommandRateLimiter.h"
#include "WorldInputQueue.h"

namespace server {

struct CommandDispatchResult {
    bool accepted;
    network::CommandType type;
    session::SessionInfo session;
    std::string error;
    PlayerSessionKey playerSessionKey;
};

class ServerCommandDispatcher {
public:
    explicit ServerCommandDispatcher(session::SessionRegistry& registry);
    ServerCommandDispatcher(session::SessionRegistry& registry,
                            WorldInputQueue& inputQueue);
    ServerCommandDispatcher(session::SessionRegistry& registry,
                            ChallengeSessionLoginService& auth,
                            GameplaySessionPort& gameplaySessions);
    void bindWorldInputQueue(WorldInputQueue& inputQueue);
    void beginFrame(uint64_t worldTick);
    void forgetSession(int64_t sessionId);
    void clearRateLimits();

    CommandDispatchResult dispatch(const network::NetworkCommand& command);
    session::SessionRegistry& sessionRegistry();
    std::size_t snapshotRequestCount() const;
    std::vector<CommandDispatchResult> dispatchAll(
        const std::vector<network::NetworkCommand>& commands);

private:
    CommandDispatchResult dispatchLogin(const network::NetworkCommand& command);
    bool usesChallengeLogin() const;

    LoginCommandHandler loginHandler;
    WorldInputQueue* inputQueue;
    CommandRateLimiter rateLimiter;
    std::size_t snapshotRequests;
    ChallengeSessionLoginService* challengeAuth;
    GameplaySessionPort* gameplaySessions;
};

}

#endif

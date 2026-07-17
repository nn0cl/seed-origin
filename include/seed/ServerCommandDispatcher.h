#ifndef SEED_SERVER_COMMAND_DISPATCHER_H
#define SEED_SERVER_COMMAND_DISPATCHER_H

#include <string>
#include <vector>

#include "LoginCommandHandler.h"
#include "WorldInputQueue.h"

namespace server {

struct CommandDispatchResult {
    bool accepted;
    network::CommandType type;
    session::SessionInfo session;
    std::string error;
};

class ServerCommandDispatcher {
public:
    explicit ServerCommandDispatcher(session::SessionRegistry& registry);
    ServerCommandDispatcher(session::SessionRegistry& registry,
                            WorldInputQueue& inputQueue);
    void bindWorldInputQueue(WorldInputQueue& inputQueue);

    CommandDispatchResult dispatch(const network::NetworkCommand& command);
    session::SessionRegistry& sessionRegistry();
    std::vector<CommandDispatchResult> dispatchAll(
        const std::vector<network::NetworkCommand>& commands);

private:
    LoginCommandHandler loginHandler;
    WorldInputQueue* inputQueue;
};

}

#endif

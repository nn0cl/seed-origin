#include "ServerCommandDispatcher.h"
#include "ChatCommandHandler.h"

namespace server {

namespace {

session::SessionInfo emptySession() {
    return {0, 0, std::string(), false};
}

}

ServerCommandDispatcher::ServerCommandDispatcher(session::SessionRegistry& registry)
    : loginHandler(registry), inputQueue(0) {}

ServerCommandDispatcher::ServerCommandDispatcher(session::SessionRegistry& registry,
                                                 WorldInputQueue& queue)
    : loginHandler(registry), inputQueue(&queue) {}

void ServerCommandDispatcher::bindWorldInputQueue(WorldInputQueue& queue) {
    inputQueue = &queue;
}

session::SessionRegistry& ServerCommandDispatcher::sessionRegistry() {
    return loginHandler.sessionRegistry();
}

CommandDispatchResult ServerCommandDispatcher::dispatch(
    const network::NetworkCommand& command) {
    CommandDispatchResult result = {false, command.type, emptySession(), std::string()};

    if (command.type == network::CommandType::Login) {
        const LoginResult login = loginHandler.handle(command);
        result.accepted = login.accepted;
        result.session = login.session;
        result.error = login.error;
        return result;
    }

    if (command.type == network::CommandType::Chat) {
        if (inputQueue == 0) {
            result.error = "chat world input queue is not bound";
            return result;
        }
        if (!loginHandler.sessionRegistry().isActive(command.sessionId)) {
            result.error = "chat requires an active anonymous session";
            return result;
        }
        ChatCommandHandler handler(*inputQueue);
        result.accepted = handler.handle(command, result.error);
        return result;
    }

    result.error = "command handler is not implemented";
    return result;
}

std::vector<CommandDispatchResult> ServerCommandDispatcher::dispatchAll(
    const std::vector<network::NetworkCommand>& commands) {
    std::vector<CommandDispatchResult> results;
    results.reserve(commands.size());
    for (std::vector<network::NetworkCommand>::const_iterator it = commands.begin();
         it != commands.end(); ++it) {
        results.push_back(dispatch(*it));
    }
    return results;
}

}

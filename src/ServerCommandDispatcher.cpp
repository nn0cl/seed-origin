#include "ServerCommandDispatcher.h"

namespace server {

namespace {

session::SessionInfo emptySession() {
    return {0, 0, std::string(), false};
}

}

ServerCommandDispatcher::ServerCommandDispatcher(session::SessionRegistry& registry)
    : loginHandler(registry) {}

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

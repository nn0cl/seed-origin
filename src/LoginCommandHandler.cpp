#include "LoginCommandHandler.h"

namespace server {

LoginCommandHandler::LoginCommandHandler(session::SessionRegistry& registry)
    : registry(registry) {}

LoginResult LoginCommandHandler::handle(const network::NetworkCommand& command) {
    LoginResult result = {false, std::string(), {0, 0, std::string(), false}};
    if (command.type != network::CommandType::Login) {
        result.error = "command is not a login";
        return result;
    }

    if (!network::validateCommand(command, result.error)) return result;

    result.session = registry.login(command.payload);
    if (result.session.internalId == 0) {
        result.error = "session id allocation failed";
        return result;
    }
    result.accepted = true;
    return result;
}

}

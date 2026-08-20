#include "DisconnectCommandHandler.h"
#include "FieldSessionPresence.h"

namespace server {

DisconnectCommandHandler::DisconnectCommandHandler(
    session::SessionRegistry& registry)
    : registry(registry) {}

DisconnectResult DisconnectCommandHandler::handle(
    const network::NetworkCommand& command) {
    DisconnectResult result = {false, std::string(),
                               {0, 0, std::string(), false}};
    if (command.type != network::CommandType::Disconnect) {
        result.error = "command is not a disconnect";
        return result;
    }
    if (!network::validateCommand(command, result.error)) return result;
    if (!registry.isActive(command.sessionId)) {
        result.error = "disconnect requires an active anonymous session";
        return result;
    }

    FieldSessionPresence::removeAfterLogout(command.sessionId);
    if (!registry.logout(command.sessionId)) {
        result.error = "disconnect could not end the session";
        return result;
    }
    result.session.internalId = command.sessionId;
    result.accepted = true;
    return result;
}

}

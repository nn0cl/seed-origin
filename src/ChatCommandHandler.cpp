#include "ChatCommandHandler.h"

namespace server {

ChatCommandHandler::ChatCommandHandler(WorldInputQueue& queue) : inputQueue(queue) {}

bool ChatCommandHandler::handle(const network::NetworkCommand& command,
                                std::string& error) {
    if (command.type != network::CommandType::Chat || command.sessionId <= 0) {
        error = "command is not an authenticated chat command";
        return false;
    }
    if (!network::validateCommand(command, error)) return false;
    const std::string::size_type separator = command.payload.find('|');
    if (separator == std::string::npos) {
        error = "chat command must contain audience and message";
        return false;
    }
    const std::string audience = command.payload.substr(0, separator);
    const std::string message = command.payload.substr(separator + 1);
    if (!inputQueue.enqueueChat(command.sessionId, audience, message)) {
        error = "chat input is invalid or world input queue is full";
        return false;
    }
    error.clear();
    return true;
}

}

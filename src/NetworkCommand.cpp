#include "NetworkCommand.h"

namespace network {

namespace {
bool isKnownCommandType(CommandType type) {
    switch (type) {
        case CommandType::Login:
        case CommandType::Move:
        case CommandType::Chat:
        case CommandType::Attack:
        case CommandType::CastSpell:
        case CommandType::Disconnect:
            return true;
    }
    return false;
}
}

bool validateCommand(const NetworkCommand& command, std::string& error) {
    if (command.version != CURRENT_PROTOCOL_VERSION) {
        error = "unsupported protocol version";
        return false;
    }
    if (!isKnownCommandType(command.type)) {
        error = "unknown command type";
        return false;
    }
    if (command.payload.size() > MAX_COMMAND_PAYLOAD) {
        error = "command payload too large";
        return false;
    }
    if (command.type == CommandType::Login) {
        if (command.sessionId != 0) {
            error = "login must not provide an internal session id";
            return false;
        }
    } else if (command.sessionId <= 0) {
        error = "command requires an active session id";
        return false;
    }
    error.clear();
    return true;
}

}

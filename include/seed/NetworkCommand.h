#ifndef SEED_NETWORK_COMMAND_H
#define SEED_NETWORK_COMMAND_H

#include <stdint.h>
#include <stddef.h>
#include <string>

namespace network {

static const uint16_t CURRENT_PROTOCOL_VERSION = 1;
static const size_t MAX_COMMAND_PAYLOAD = 4096;

enum class CommandType {
    Login = 1,
    Move = 2,
    Chat = 3,
    Attack = 4,
    CastSpell = 5,
    Disconnect = 6
};

struct NetworkCommand {
    uint16_t version;
    CommandType type;
    int64_t sessionId;
    std::string payload;
};

bool validateCommand(const NetworkCommand& command, std::string& error);

}

#endif

#ifndef SEED_NETWORK_COMMAND_H
#define SEED_NETWORK_COMMAND_H

#include <cstddef>
#include <cstdint>
#include <string>

#include "Protocol.h"

namespace network {

inline constexpr std::size_t MAX_COMMAND_PAYLOAD = 4096;

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

[[nodiscard]] bool validateCommand(const NetworkCommand& command, std::string& error);

}

#endif

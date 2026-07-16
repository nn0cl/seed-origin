#include <cassert>

#include "NetworkCommand.h"

namespace network_command_tests {

void validates_login_without_client_internal_id() {
    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "player-a"
    };
    std::string error;
    assert(network::validateCommand(command, error));
    assert(error.empty());
}

void rejects_invalid_session_and_oversized_payload() {
    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Chat,
        0,
        std::string(network::MAX_COMMAND_PAYLOAD + 1, 'x')
    };
    std::string error;
    assert(!network::validateCommand(command, error));
    assert(!error.empty());
}

} // namespace network_command_tests

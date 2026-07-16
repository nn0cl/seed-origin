#include <cassert>

#include "ServerRuntime.h"

namespace server_runtime_tests {

void rejects_commands_before_start() {
    server::ServerRuntime runtime;
    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "player-a"
    };
    assert(!runtime.submit(command));
    assert(runtime.pendingCommandCount() == 0);
}

void drains_valid_commands_in_fifo_order() {
    server::ServerRuntime runtime;
    network::NetworkCommand first = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "first"
    };
    network::NetworkCommand second = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "second"
    };
    assert(runtime.start(0));
    assert(runtime.submit(first));
    assert(runtime.submit(second));
    const std::vector<network::NetworkCommand> commands = runtime.drainCommands();
    assert(commands.size() == 2);
    assert(commands[0].payload == "first");
    assert(commands[1].payload == "second");
    assert(runtime.stop());
}

} // namespace server_runtime_tests

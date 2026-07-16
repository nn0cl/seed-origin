#include "ServerCommandDispatcher.h"

#include <cassert>

namespace server_command_dispatcher_tests {

void accepts_login() {
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0, "player"};

    const server::CommandDispatchResult result = dispatcher.dispatch(command);
    assert(result.accepted);
    assert(result.session.internalId != 0);
}

void rejects_unimplemented_command() {
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Move, 0, "1,0,0"};

    const server::CommandDispatchResult result = dispatcher.dispatch(command);
    assert(!result.accepted);
    assert(result.error == "command handler is not implemented");
}

} // namespace server_command_dispatcher_tests

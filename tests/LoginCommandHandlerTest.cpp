#include <cassert>

#include "LoginCommandHandler.h"

namespace login_command_handler_tests {

void creates_temporary_session_from_valid_login() {
    session::SessionRegistry registry;
    server::LoginCommandHandler handler(registry);
    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "player-a"
    };
    const server::LoginResult result = handler.handle(command);
    assert(result.accepted);
    assert(result.session.internalId > 0);
    assert(!result.session.authenticated);
}

void rejects_client_supplied_internal_id() {
    session::SessionRegistry registry;
    server::LoginCommandHandler handler(registry);
    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        42,
        "player-a"
    };
    const server::LoginResult result = handler.handle(command);
    assert(!result.accepted);
    assert(result.session.internalId == 0);
}

} // namespace login_command_handler_tests

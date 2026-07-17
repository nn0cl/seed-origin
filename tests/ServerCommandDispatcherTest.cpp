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

void routes_combat_only_for_an_active_session() {
    session::SessionRegistry registry;
    server::WorldInputQueue queue;
    server::ServerCommandDispatcher dispatcher(registry, queue);
    const session::SessionInfo session = registry.login("player");
    const network::NetworkCommand attack = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Attack,
        session.internalId, "attack-1|2,100"};
    const server::CommandDispatchResult accepted = dispatcher.dispatch(attack);
    assert(accepted.accepted);
    assert(queue.pendingCount() == 1);

    const network::NetworkCommand forged = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::CastSpell,
        9999, "spell-1|2,fire,100"};
    const server::CommandDispatchResult rejected = dispatcher.dispatch(forged);
    assert(!rejected.accepted);
    assert(queue.pendingCount() == 1);
}

} // namespace server_command_dispatcher_tests

#include <cassert>

#include "CombatCommandHandler.h"

namespace combat_command_handler_tests {

void queues_attack_and_spell_intents() {
    server::WorldInputQueue queue;
    server::CombatCommandHandler handler(queue);
    const network::NetworkCommand attack = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Attack, 10, "20,100"};
    const network::NetworkCommand spell = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::CastSpell, 10,
        "20,fire,120"};
    assert(handler.handle(attack).accepted);
    assert(handler.handle(spell).accepted);
    const std::vector<server::WorldInput> inputs = queue.takeFrame();
    assert(inputs.size() == 2);
    assert(inputs[0].kind() == server::WorldInputKind::Combat);
    assert(inputs[1].kind() == server::WorldInputKind::Spell);
    assert(inputs[0].sequence() < inputs[1].sequence());
}

void rejects_malformed_or_oversized_power() {
    server::WorldInputQueue queue;
    server::CombatCommandHandler handler(queue);
    const network::NetworkCommand malformed = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Attack, 10, "20"};
    const network::NetworkCommand oversized = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::CastSpell, 10,
        "20,fire,100001"};
    assert(!handler.handle(malformed).accepted);
    assert(!handler.handle(oversized).accepted);
    assert(queue.pendingCount() == 0);
}

} // namespace combat_command_handler_tests

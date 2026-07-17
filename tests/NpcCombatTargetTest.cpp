#include <cassert>

#include "WorldFrameApplier.h"

namespace npc_combat_target_tests {

void applies_combat_damage_to_a_live_npc_target() {
    Field* field = Field::getInstance();
    assert(field->setPlayer(Player(9501, Status(100, 50),
                                   Position(9501, 0, 0, 0))));
    assert(field->setNpc(Npc(19501, "goblin", Status(80, 0),
                             Position(19501, 3, 0, 0))));

    server::WorldInputQueue queue;
    assert(queue.enqueueCombat(9501, 19501, 25.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    assert(field->findNpc(19501)->getStatus().getHp() == 55);
}

void rejects_damage_to_a_dead_npc_without_mutation() {
    Field* field = Field::getInstance();
    assert(field->setPlayer(Player(9502, Status(100, 50),
                                   Position(9502, 0, 0, 0))));
    assert(field->setNpc(Npc(19502, "goblin", Status(0, 0),
                             Position(19502, 3, 0, 0))));

    server::WorldInputQueue queue;
    assert(queue.enqueueCombat(9502, 19502, 25.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(!applier.apply(frame, updates, error));
    assert(field->findNpc(19502)->getStatus().getHp() == 0);
}

} // namespace npc_combat_target_tests

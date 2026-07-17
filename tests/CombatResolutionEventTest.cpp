#include <cassert>

#include "WorldFrameApplier.h"

namespace combat_resolution_event_tests {

void emits_authoritative_combat_and_spell_results() {
    Field* field = Field::getInstance();
    assert(field->setPlayer(Player(1001, Status(100, 50), Position(1001, 0, 0, 0))));
    assert(field->setPlayer(Player(1002, Status(500, 50), Position(1002, 3, 0, 0))));
    server::WorldInputQueue queue;
    assert(queue.enqueueCombat(1001, 1002, 20.0f));
    assert(queue.enqueueSpell(1001, 1002, "fire", 30.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    bool sawCombat = false;
    bool sawSpell = false;
    for (std::vector<network::WorldUpdate>::const_iterator it = updates.begin();
         it != updates.end(); ++it) {
        sawCombat = sawCombat || it->payload.find("combatResult=") == 0;
        sawSpell = sawSpell || it->payload.find("spellResult=") == 0;
    }
    assert(sawCombat && sawSpell);
}

} // namespace combat_resolution_event_tests

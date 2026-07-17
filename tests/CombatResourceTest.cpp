#include <cassert>

#include "WorldFrameApplier.h"

namespace combat_resource_tests {

void charges_spell_mp_and_publishes_cooldown() {
    Field* field = Field::getInstance();
    assert(field->setPlayer(Player(9401, Status(100, 10), Position(9401, 0, 0, 0))));
    assert(field->setPlayer(Player(9402, Status(100, 10), Position(9402, 3, 0, 0))));
    server::WorldInputQueue queue;
    assert(queue.enqueueSpell(9401, 9402, "fire", 50.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    assert(field->findPlayer(9401)->getStatus().getMp() == 5);
    bool published = false;
    for (std::vector<network::WorldUpdate>::const_iterator it = updates.begin();
         it != updates.end(); ++it) {
        published = published ||
            (it->payload.find("spellResult=") == 0 &&
             it->payload.find("mpSpent:5") != std::string::npos &&
             it->payload.find("cooldownUntil:3") != std::string::npos);
    }
    assert(published);
}

void rejects_same_frame_spell_repetition_without_resource_mutation() {
    Field* field = Field::getInstance();
    assert(field->setPlayer(Player(9411, Status(100, 20), Position(9411, 0, 0, 0))));
    assert(field->setPlayer(Player(9412, Status(100, 20), Position(9412, 3, 0, 0))));
    server::WorldInputQueue queue;
    assert(queue.enqueueSpell(9411, 9412, "fire", 10.0f));
    assert(queue.enqueueSpell(9411, 9412, "fire", 10.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(!applier.apply(frame, updates, error));
    assert(field->findPlayer(9411)->getStatus().getMp() == 20);
    assert(field->findPlayer(9412)->getStatus().getHp() == 100);
}

} // namespace combat_resource_tests

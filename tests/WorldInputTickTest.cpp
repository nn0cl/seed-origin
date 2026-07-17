#include <cassert>

#include "WorldInputTick.h"
#include "WorldFrameUpdateBuilder.h"
#include "WorldUpdate.h"

namespace world_input_tick_tests {

void advances_world_tick_and_cuts_unified_inputs() {
    server::WorldInputQueue queue;
    server::WorldInputTick tick(queue);
    const Status status;
    Player player;
    assert(tick.submitAction(Action(2, &player, nullptr, status)));
    assert(tick.submitMovement(42, 1.0f, 0.0f, 0.0f));
    const server::WorldFrameInputs frame = tick.advanceFrame();
    assert(frame.worldTick == 1);
    assert(frame.inputs.size() == 2);
    assert(frame.inputs[0].sequence() < frame.inputs[1].sequence());
    assert(tick.currentWorldTick() == 1);
    assert(queue.pendingCount() == 0);
}

void emits_inputs_in_common_sequence_order() {
    server::WorldInputQueue queue;
    server::WorldInputTick tick(queue);
    const Status status;
    Player player;
    assert(tick.submitMovement(42, 1.0f, 0.0f, 0.0f));
    assert(tick.submitAction(Action(2, &player, nullptr, status)));
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameUpdateBuilder builder;
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(builder.build(frame, updates, error));
    assert(updates.size() == 2);
    assert(updates[0].payload.find("movement=") == 0);
    assert(updates[1].payload.find("action=") == 0);
    assert(updates[0].sequence < updates[1].sequence);
}

} // namespace world_input_tick_tests

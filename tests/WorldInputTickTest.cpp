#include <cassert>

#include "WorldInputTick.h"

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

} // namespace world_input_tick_tests

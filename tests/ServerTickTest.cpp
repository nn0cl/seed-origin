#include <cassert>

#include "ServerTick.h"

namespace server_tick_tests {

void advances_fixed_logical_frames_and_cuts_queue() {
    ActionQueue queue;
    ServerTick tick(queue);
    const Status status;
    Player player;
    const Action action(2, &player, nullptr, status);

    assert(tick.currentWorldTick() == 0);
    assert(tick.submit(action));
    const FrameActions first = tick.advanceFrame();
    assert(first.worldTick == 1);
    assert(first.actions.size() == 1);
    assert(tick.currentWorldTick() == 1);

    const FrameActions second = tick.advanceFrame();
    assert(second.worldTick == 2);
    assert(second.actions.empty());
}

} // namespace server_tick_tests

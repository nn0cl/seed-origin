#include <cassert>

#include "WorldInputQueue.h"

namespace world_input_queue_tests {

void preserves_order_across_action_and_movement_inputs() {
    server::WorldInputQueue queue;
    const Status status;
    Player player;
    assert(queue.enqueueMovement(42, 1.0f, 0.0f, 0.0f));
    assert(queue.enqueueAction(Action(2, &player, nullptr, status)));
    const std::vector<server::WorldInput> frame = queue.takeFrame();
    assert(frame.size() == 2);
    assert(frame[0].kind() == server::WorldInputKind::Movement);
    assert(frame[1].kind() == server::WorldInputKind::Action);
    assert(frame[0].sequence() < frame[1].sequence());
}

void rejects_invalid_inputs_without_queue_mutation() {
    server::WorldInputQueue queue;
    const Status status;
    Player player;
    assert(!queue.enqueueAction(Action(99, &player, nullptr, status)));
    assert(!queue.enqueueMovement(0, 1.0f, 0.0f, 0.0f));
    assert(queue.pendingCount() == 0);
}

} // namespace world_input_queue_tests

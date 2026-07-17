#include <cassert>

#include "ActionQueue.h"

namespace action_queue_tests {

void assigns_arrival_sequence_and_preserves_order() {
    ActionQueue queue;
    const Status status;
    Player firstPlayer;
    Player secondPlayer;
    const Action first(2, &firstPlayer, nullptr, status);
    const Action second(2, &secondPlayer, nullptr, status);

    assert(queue.enqueue(first));
    assert(queue.enqueue(second));
    const std::vector<QueuedAction> frame = queue.takeFrame();
    assert(frame.size() == 2);
    assert(frame[0].getSequence() < frame[1].getSequence());
    assert(queue.pendingCount() == 0);
}

void arrivals_after_frame_cutover_are_deferred() {
    ActionQueue queue;
    const Status status;
    Player player;
    const Action action(2, &player, nullptr, status);

    assert(queue.enqueue(action));
    const std::vector<QueuedAction> firstFrame = queue.takeFrame();
    assert(firstFrame.size() == 1);
    assert(queue.enqueue(action));
    assert(queue.pendingCount() == 1);
}

void rejects_invalid_actions() {
    ActionQueue queue;
    const Status status;
    const Action invalid(99, nullptr, nullptr, status);
    assert(!queue.enqueue(invalid));
    assert(queue.pendingCount() == 0);
}

} // namespace action_queue_tests

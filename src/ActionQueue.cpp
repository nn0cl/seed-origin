#include "ActionQueue.h"

#include <limits>

QueuedAction::QueuedAction(uint64_t sequence, const Action& action)
    : sequence(sequence), action(action) {}

uint64_t QueuedAction::getSequence() const {
    return sequence;
}

const Action& QueuedAction::getAction() const {
    return action;
}

ActionQueue::ActionQueue() : nextSequence(1) {}

bool ActionQueue::enqueue(const Action& action) {
    if (!action.isValid()) return false;
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_ACTIONS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(QueuedAction(nextSequence, action));
    ++nextSequence;
    return true;
}

std::vector<QueuedAction> ActionQueue::takeFrame() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<QueuedAction> frame;
    frame.reserve(pending.size());
    while (!pending.empty()) {
        frame.push_back(pending.front());
        pending.pop_front();
    }
    return frame;
}

size_t ActionQueue::pendingCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return pending.size();
}

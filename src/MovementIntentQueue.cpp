#include "MovementIntentQueue.h"

#include <cmath>
#include <limits>

namespace server {

MovementIntentQueue::MovementIntentQueue() : nextSequence(1) {}

bool MovementIntentQueue::enqueue(int64_t sessionId, float dx, float dy, float dz) {
    if (sessionId <= 0 || !std::isfinite(dx) || !std::isfinite(dy) ||
        !std::isfinite(dz) || pending.size() >= MAX_PENDING_INTENTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) {
        return false;
    }
    pending.push_back({nextSequence++, sessionId, dx, dy, dz});
    return true;
}

std::vector<MovementIntent> MovementIntentQueue::takeFrame() {
    std::vector<MovementIntent> frame;
    frame.reserve(pending.size());
    while (!pending.empty()) {
        frame.push_back(pending.front());
        pending.pop_front();
    }
    return frame;
}

size_t MovementIntentQueue::pendingCount() const {
    return pending.size();
}

}

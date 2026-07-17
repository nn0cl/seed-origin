#include "MovementIntentQueue.h"

#include <cmath>
#include <limits>

namespace server {

MovementIntentQueue::MovementIntentQueue() : nextSequence(1) {}

bool isValidMovementDelta(float dx, float dy, float dz) {
    if (!std::isfinite(dx) || !std::isfinite(dy) || !std::isfinite(dz)) return false;
    return dx * dx + dy * dy + dz * dz <=
           MAX_MOVE_DISTANCE_PER_FRAME * MAX_MOVE_DISTANCE_PER_FRAME;
}

bool MovementIntentQueue::enqueue(int64_t sessionId, float dx, float dy, float dz) {
    if (sessionId <= 0 || !isValidMovementDelta(dx, dy, dz) ||
        pending.size() >= MAX_PENDING_INTENTS ||
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

bool MovementIntentQueue::restoreFrame(const std::vector<MovementIntent>& frame) {
    if (frame.size() > MAX_PENDING_INTENTS - pending.size()) return false;
    for (std::vector<MovementIntent>::const_reverse_iterator it = frame.rbegin();
         it != frame.rend(); ++it) {
        pending.push_front(*it);
    }
    return true;
}

size_t MovementIntentQueue::pendingCount() const {
    return pending.size();
}

}

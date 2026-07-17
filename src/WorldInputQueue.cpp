#include "WorldInputQueue.h"

#include <cmath>
#include <limits>

namespace server {

WorldInput::WorldInput(uint64_t sequence, const Action& action)
    : inputSequence(sequence), inputKind(WorldInputKind::Action), inputAction(action),
      inputMovement({0, 0, 0.0f, 0.0f, 0.0f}) {}

WorldInput::WorldInput(uint64_t sequence, const MovementIntent& movement)
    : inputSequence(sequence), inputKind(WorldInputKind::Movement), inputAction(0, nullptr, nullptr, Status()),
      inputMovement(movement) {}

uint64_t WorldInput::sequence() const { return inputSequence; }
WorldInputKind WorldInput::kind() const { return inputKind; }
const Action& WorldInput::action() const { return inputAction; }
const MovementIntent& WorldInput::movement() const { return inputMovement; }

WorldInputQueue::WorldInputQueue() : nextSequence(1) {}

bool WorldInputQueue::enqueueAction(const Action& action) {
    if (!action.isValid()) return false;
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(WorldInput(nextSequence++, action));
    return true;
}

bool WorldInputQueue::enqueueMovement(int64_t sessionId, float dx, float dy, float dz) {
    if (sessionId <= 0 || !std::isfinite(dx) || !std::isfinite(dy) ||
        !std::isfinite(dz)) return false;
    std::lock_guard<std::mutex> lock(mutex);
    if (pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) return false;
    pending.push_back(WorldInput(nextSequence++, {0, sessionId, dx, dy, dz}));
    return true;
}

std::vector<WorldInput> WorldInputQueue::takeFrame() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<WorldInput> frame;
    frame.reserve(pending.size());
    while (!pending.empty()) {
        frame.push_back(pending.front());
        pending.pop_front();
    }
    return frame;
}

size_t WorldInputQueue::pendingCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return pending.size();
}

}

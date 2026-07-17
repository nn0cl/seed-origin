#include "NpcAi.h"

#include "MovementIntentQueue.h"

#include <cmath>
#include <limits>

namespace server {

namespace {
bool validPosition(const Position& position) {
    return std::isfinite(position.getX()) &&
           std::isfinite(position.getY()) &&
           std::isfinite(position.getZ());
}
}

bool NpcAiPlanner::plan(const NpcAiObservation& observation,
                        NpcAiDecision& decision) {
    if (observation.npcId <= 0 || !observation.npcAlive ||
        !std::isfinite(observation.step) || observation.step <= 0.0f ||
        observation.step > MAX_MOVE_DISTANCE_PER_FRAME ||
        !validPosition(observation.npcPosition) ||
        !validPosition(observation.targetPosition)) {
        return false;
    }

    const float dx = observation.targetPosition.getX() -
                     observation.npcPosition.getX();
    const float dy = observation.targetPosition.getY() -
                     observation.npcPosition.getY();
    const float dz = observation.targetPosition.getZ() -
                     observation.npcPosition.getZ();
    if (!std::isfinite(dx) || !std::isfinite(dy) || !std::isfinite(dz)) {
        return false;
    }

    decision = {observation.npcId, 0.0f, 0.0f, 0.0f};
    const float absX = std::fabs(dx);
    const float absY = std::fabs(dy);
    const float absZ = std::fabs(dz);
    if (absX >= absY && absX >= absZ && absX > 0.0f) {
        decision.dx = dx > 0.0f ? observation.step : -observation.step;
    } else if (absY >= absZ && absY > 0.0f) {
        decision.dy = dy > 0.0f ? observation.step : -observation.step;
    } else if (absZ > 0.0f) {
        decision.dz = dz > 0.0f ? observation.step : -observation.step;
    }
    return isValidMovementDelta(decision.dx, decision.dy, decision.dz);
}

NpcAiInputQueue::NpcAiInputQueue() : nextSequence(1) {}

bool NpcAiInputQueue::enqueue(const NpcAiDecision& decision) {
    if (decision.npcId <= 0 || !isValidMovementDelta(decision.dx, decision.dy,
                                                     decision.dz) ||
        pending.size() >= MAX_PENDING_INPUTS ||
        nextSequence == std::numeric_limits<uint64_t>::max()) {
        return false;
    }
    pending.push_back({nextSequence++, decision});
    return true;
}

std::vector<NpcAiInput> NpcAiInputQueue::takeFrame() {
    std::vector<NpcAiInput> frame;
    frame.reserve(pending.size());
    while (!pending.empty()) {
        frame.push_back(pending.front());
        pending.pop_front();
    }
    return frame;
}

bool NpcAiInputQueue::restoreFrame(const std::vector<NpcAiInput>& frame) {
    if (frame.size() > MAX_PENDING_INPUTS - pending.size()) return false;
    for (std::vector<NpcAiInput>::const_reverse_iterator it = frame.rbegin();
         it != frame.rend(); ++it) {
        pending.push_front(*it);
    }
    return true;
}

std::size_t NpcAiInputQueue::pendingCount() const {
    return pending.size();
}

}

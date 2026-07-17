#ifndef SEED_MOVEMENT_INTENT_QUEUE_H
#define SEED_MOVEMENT_INTENT_QUEUE_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

namespace server {

inline constexpr float MAX_MOVE_DISTANCE_PER_FRAME = 100.0f;
inline constexpr float MAX_WORLD_COORDINATE = 1000000.0f;

[[nodiscard]] bool isValidMovementDelta(float dx, float dy, float dz);

struct MovementIntent {
    uint64_t sequence;
    int64_t sessionId;
    float dx;
    float dy;
    float dz;
};

class MovementIntentQueue {
public:
    inline static constexpr std::size_t MAX_PENDING_INTENTS = 4096;

    MovementIntentQueue();
    bool enqueue(int64_t sessionId, float dx, float dy, float dz);
    std::vector<MovementIntent> takeFrame();
    bool restoreFrame(const std::vector<MovementIntent>& frame);
    size_t pendingCount() const;

private:
    std::deque<MovementIntent> pending;
    uint64_t nextSequence;
};

}

#endif

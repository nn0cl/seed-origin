#ifndef SEED_MOVEMENT_INTENT_QUEUE_H
#define SEED_MOVEMENT_INTENT_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <deque>
#include <vector>

namespace server {

struct MovementIntent {
    uint64_t sequence;
    int64_t sessionId;
    float dx;
    float dy;
    float dz;
};

class MovementIntentQueue {
public:
    static const size_t MAX_PENDING_INTENTS = 4096;

    MovementIntentQueue();
    bool enqueue(int64_t sessionId, float dx, float dy, float dz);
    std::vector<MovementIntent> takeFrame();
    size_t pendingCount() const;

private:
    std::deque<MovementIntent> pending;
    uint64_t nextSequence;
};

}

#endif

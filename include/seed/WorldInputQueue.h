#ifndef SEED_WORLD_INPUT_QUEUE_H
#define SEED_WORLD_INPUT_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <deque>
#include <mutex>
#include <vector>

#include "Action.h"
#include "MovementIntentQueue.h"

namespace server {

enum class WorldInputKind {
    Action,
    Movement
};

class WorldInput {
public:
    WorldInput(uint64_t sequence, const Action& action);
    WorldInput(uint64_t sequence, const MovementIntent& movement);

    uint64_t sequence() const;
    WorldInputKind kind() const;
    const Action& action() const;
    const MovementIntent& movement() const;

private:
    uint64_t inputSequence;
    WorldInputKind inputKind;
    Action inputAction;
    MovementIntent inputMovement;
};

class WorldInputQueue {
public:
    static const size_t MAX_PENDING_INPUTS = 8192;

    WorldInputQueue();
    bool enqueueAction(const Action& action);
    bool enqueueMovement(int64_t sessionId, float dx, float dy, float dz);
    std::vector<WorldInput> takeFrame();
    size_t pendingCount() const;

private:
    mutable std::mutex mutex;
    std::deque<WorldInput> pending;
    uint64_t nextSequence;
};

}

#endif

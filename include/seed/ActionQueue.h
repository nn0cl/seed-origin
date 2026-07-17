#ifndef SEED_ACTION_QUEUE_H
#define SEED_ACTION_QUEUE_H

#include <deque>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <mutex>

#include "Action.h"

class QueuedAction {
public:
    QueuedAction(uint64_t sequence, const Action& action);

    uint64_t getSequence() const;
    const Action& getAction() const;

private:
    uint64_t sequence;
    Action action;
};

class ActionQueue {
public:
    inline static constexpr std::uint64_t FRAME_DURATION_MILLISECONDS = 50;
    inline static constexpr std::size_t MAX_PENDING_ACTIONS = 4096;

    ActionQueue();

    bool enqueue(const Action& action);
    std::vector<QueuedAction> takeFrame();
    size_t pendingCount() const;

private:
    mutable std::mutex mutex;
    std::deque<QueuedAction> pending;
    uint64_t nextSequence;
};

#endif

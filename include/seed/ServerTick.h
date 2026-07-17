#ifndef SEED_SERVER_TICK_H
#define SEED_SERVER_TICK_H

#include <stdint.h>
#include <vector>

#include "ActionQueue.h"

struct FrameActions {
    uint64_t worldTick;
    std::vector<QueuedAction> actions;
};

class ServerTick {
public:
    static const uint64_t FRAME_DURATION_MILLISECONDS = 50;

    explicit ServerTick(ActionQueue& actionQueue);

    bool submit(const Action& action);
    FrameActions advanceFrame();
    uint64_t currentWorldTick() const;

private:
    ActionQueue& actionQueue;
    uint64_t worldTick;
};

#endif

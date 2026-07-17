#ifndef SEED_WORLD_INPUT_TICK_H
#define SEED_WORLD_INPUT_TICK_H

#include <cstdint>
#include <vector>

#include "WorldInputQueue.h"

namespace server {

struct WorldFrameInputs {
    uint64_t worldTick;
    std::vector<WorldInput> inputs;
};

class WorldInputTick {
public:
    explicit WorldInputTick(WorldInputQueue& inputQueue);

    bool submitAction(const Action& action);
    bool submitMovement(int64_t sessionId, float dx, float dy, float dz);
    WorldFrameInputs advanceFrame();
    uint64_t currentWorldTick() const;

private:
    WorldInputQueue& inputQueue;
    uint64_t worldTick;
};

}

#endif

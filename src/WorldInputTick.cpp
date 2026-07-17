#include "WorldInputTick.h"

namespace server {

WorldInputTick::WorldInputTick(WorldInputQueue& inputQueue)
    : inputQueue(inputQueue), worldTick(0) {}

bool WorldInputTick::submitAction(const Action& action) {
    return inputQueue.enqueueAction(action);
}

bool WorldInputTick::submitMovement(int64_t sessionId, float dx, float dy, float dz) {
    return inputQueue.enqueueMovement(sessionId, dx, dy, dz);
}

WorldFrameInputs WorldInputTick::advanceFrame() {
    ++worldTick;
    WorldFrameInputs frame = {worldTick, inputQueue.takeFrame()};
    return frame;
}

uint64_t WorldInputTick::currentWorldTick() const {
    return worldTick;
}

}

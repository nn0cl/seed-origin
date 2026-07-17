#include "ServerTick.h"

ServerTick::ServerTick(ActionQueue& actionQueue)
    : actionQueue(actionQueue), worldTick(0) {}

bool ServerTick::submit(const Action& action) {
    return actionQueue.enqueue(action);
}

FrameActions ServerTick::advanceFrame() {
    ++worldTick;
    FrameActions frame = {worldTick, actionQueue.takeFrame()};
    return frame;
}

uint64_t ServerTick::currentWorldTick() const {
    return worldTick;
}

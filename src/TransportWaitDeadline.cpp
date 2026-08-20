#include "TransportWaitDeadline.h"

#include "MonotonicClockPort.h"

namespace client {

TransportWaitDeadline::TransportWaitDeadline() : deadlineMs(0) {}

void TransportWaitDeadline::clear() {
    deadlineMs = 0;
}

void TransportWaitDeadline::arm(MonotonicClockPort* clock, uint64_t timeoutMs) {
    if (clock == 0 || timeoutMs == 0) {
        clear();
        return;
    }
    deadlineMs = clock->nowMs() + timeoutMs;
}

bool TransportWaitDeadline::isArmed() const {
    return deadlineMs > 0;
}

bool TransportWaitDeadline::isExpired(MonotonicClockPort* clock) const {
    if (!isArmed() || clock == 0) return false;
    return clock->nowMs() >= deadlineMs;
}

void TransportWaitDeadline::disarm() {
    deadlineMs = 0;
}

}

#ifndef SEED_TRANSPORT_WAIT_DEADLINE_H
#define SEED_TRANSPORT_WAIT_DEADLINE_H

#include <cstdint>

namespace client {

class MonotonicClockPort;

class TransportWaitDeadline {
public:
    TransportWaitDeadline();

    void clear();
    void arm(MonotonicClockPort* clock, uint64_t timeoutMs);
    bool isArmed() const;
    bool isExpired(MonotonicClockPort* clock) const;
    void disarm();

private:
    uint64_t deadlineMs;
};

}

#endif

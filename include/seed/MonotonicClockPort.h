#ifndef SEED_MONOTONIC_CLOCK_PORT_H
#define SEED_MONOTONIC_CLOCK_PORT_H

#include <cstdint>

namespace client {

class MonotonicClockPort {
public:
    virtual ~MonotonicClockPort() {}
    virtual uint64_t nowMs() const = 0;
};

}

#endif

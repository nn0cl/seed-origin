#ifndef SEED_TRANSPORT_TIMEOUTS_H
#define SEED_TRANSPORT_TIMEOUTS_H

#include <cstdint>

namespace client {

struct TransportTimeouts {
    uint64_t loginResponseWaitMs;
    uint64_t snapshotWaitMs;
};

}

#endif

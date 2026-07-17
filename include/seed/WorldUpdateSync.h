#ifndef SEED_WORLD_UPDATE_SYNC_H
#define SEED_WORLD_UPDATE_SYNC_H

#include <cstdint>
#include <string>

#include "WorldUpdateSequence.h"

namespace network {

enum class SyncDecision {
    Apply,
    IgnoreDuplicate,
    RequestSnapshot,
    Reject
};

struct SyncResult {
    SyncDecision decision;
    uint64_t expectedSequence;
    std::string error;
};

class WorldUpdateSyncController {
public:
    WorldUpdateSyncController();

    SyncResult receive(const WorldUpdate& update);
    void confirmSnapshot(uint64_t sequence);
    uint64_t expectedSequence() const;

private:
    WorldUpdateSequenceTracker tracker;
};

}

#endif

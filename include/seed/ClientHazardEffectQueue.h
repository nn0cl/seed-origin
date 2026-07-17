#ifndef SEED_CLIENT_HAZARD_EFFECT_QUEUE_H
#define SEED_CLIENT_HAZARD_EFFECT_QUEUE_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "WorldUpdateSync.h"

namespace client {

inline constexpr std::size_t MAX_HAZARD_EFFECTS = 256;

struct HazardEffect {
    uint64_t worldTick;
    uint64_t sequence;
    uint64_t eventId;
    float severity;
    float instability;
};

enum class HazardIngestResult {
    Applied,
    IgnoredDuplicate,
    RequestSnapshot,
    Rejected,
    Backpressure
};

class ClientHazardEffectQueue {
public:
    ClientHazardEffectQueue();
    HazardIngestResult ingest(const network::WorldUpdate& update,
                              std::string& error);
    void confirmSnapshot(uint64_t sequence);
    size_t drain(size_t limit, std::vector<HazardEffect>& effects);
    size_t size() const;
    const network::WorldUpdateSyncController& synchronization() const;

private:
    std::deque<HazardEffect> effects;
    network::WorldUpdateSyncController sync;
};

}

#endif

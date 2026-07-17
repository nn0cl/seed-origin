#ifndef SEED_CLIENT_WORLD_UPDATE_RECEIVER_H
#define SEED_CLIENT_WORLD_UPDATE_RECEIVER_H

#include <cstddef>
#include <string>
#include <vector>

#include "ClientEnvironmentState.h"
#include "ClientHazardEffectQueue.h"
#include "WorldUpdateAccumulator.h"
#include "ClientWorldSnapshotApplier.h"

namespace client {

enum class WorldReceiveDecision {
    Applied,
    NoData,
    RequestSnapshot,
    Rejected
};

class ClientWorldUpdateReceiver {
public:
    ClientWorldUpdateReceiver();
    bool receive(const std::vector<uint8_t>& bytes, size_t& applied,
                 std::string& error);
    void beginReconnect();
    bool snapshotRequested() const;
    uint64_t expectedSequence() const;
    WorldReceiveDecision lastDecision() const;
    const ClientEnvironmentState& environment() const;
    const ClientHazardEffectQueue& hazardEffects() const;

private:
    network::WorldUpdateAccumulator accumulator;
    ClientWorldSnapshotApplier snapshotApplier;
    ClientEnvironmentState environmentState;
    ClientHazardEffectQueue hazardQueue;
    bool needsSnapshot;
    uint64_t expected;
    WorldReceiveDecision decision;
};

}

#endif

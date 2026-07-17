#ifndef SEED_WORLD_SNAPSHOT_BUILDER_H
#define SEED_WORLD_SNAPSHOT_BUILDER_H

#include <cstdint>
#include <string>

#include "EnvironmentEther.h"
#include "WorldUpdate.h"

namespace server {

class WorldSnapshotBuilder {
public:
    WorldSnapshotBuilder();

    bool build(uint64_t worldTick, const world::EnvironmentEther& ether,
               float hazard, network::WorldUpdate& snapshot,
               std::string& error);
    uint64_t nextSequence() const;

private:
    uint64_t sequence;
};

}

#endif

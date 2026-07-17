#ifndef SEED_WORLD_FRAME_UPDATE_BUILDER_H
#define SEED_WORLD_FRAME_UPDATE_BUILDER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "ServerTick.h"
#include "WorldUpdate.h"

namespace server {

class WorldFrameUpdateBuilder {
public:
    WorldFrameUpdateBuilder();

    bool build(const FrameActions& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);
    uint64_t nextSequence() const;

private:
    uint64_t updateSequence;
    uint64_t eventId;
};

}

#endif

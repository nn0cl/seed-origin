#ifndef SEED_WORLD_FRAME_UPDATE_BUILDER_H
#define SEED_WORLD_FRAME_UPDATE_BUILDER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "ServerTick.h"
#include "WorldInputTick.h"
#include "WorldUpdate.h"

namespace server {

class WorldFrameUpdateBuilder {
public:
    WorldFrameUpdateBuilder();

    bool build(const FrameActions& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);
    bool build(const WorldFrameInputs& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);
    uint64_t nextSequence() const;

private:
    uint64_t updateSequence;
    uint64_t eventId;

    bool appendEvent(uint64_t worldTick, const std::string& payload,
                     std::vector<network::WorldUpdate>& updates,
                     std::string& error);
};

}

#endif

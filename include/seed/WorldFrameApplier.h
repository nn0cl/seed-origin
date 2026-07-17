#ifndef SEED_WORLD_FRAME_APPLIER_H
#define SEED_WORLD_FRAME_APPLIER_H

#include <string>
#include <vector>

#include "Field.h"
#include "WorldFrameUpdateBuilder.h"

namespace server {

class WorldFrameApplier {
public:
    explicit WorldFrameApplier(Field& field);

    bool apply(const FrameActions& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);

private:
    Field& field;
    WorldFrameUpdateBuilder updateBuilder;
};

}

#endif

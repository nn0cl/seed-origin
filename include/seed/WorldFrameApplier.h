#ifndef SEED_WORLD_FRAME_APPLIER_H
#define SEED_WORLD_FRAME_APPLIER_H

#include <string>
#include <vector>

#include "Field.h"
#include "MovementIntentQueue.h"
#include "WorldInputTick.h"
#include "WorldFrameUpdateBuilder.h"

namespace server {

class WorldFrameApplier {
public:
    explicit WorldFrameApplier(Field& field);
    WorldFrameApplier(Field& field, MovementIntentQueue& movementQueue);

    bool apply(const FrameActions& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);
    bool apply(const WorldFrameInputs& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);

private:
    Field& field;
    MovementIntentQueue* movementQueue;
    WorldFrameUpdateBuilder updateBuilder;
};

}

#endif

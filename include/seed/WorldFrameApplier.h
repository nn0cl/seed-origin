#ifndef SEED_WORLD_FRAME_APPLIER_H
#define SEED_WORLD_FRAME_APPLIER_H

#include <string>
#include <vector>

#include "Field.h"
#include "MovementIntentQueue.h"
#include "NpcAi.h"
#include "WorldInputTick.h"
#include "WorldFrameUpdateBuilder.h"

namespace server {

class WorldFrameApplier {
public:
    explicit WorldFrameApplier(Field& field);
    WorldFrameApplier(Field& field, MovementIntentQueue& movementQueue);
    WorldFrameApplier(Field& field, NpcAiInputQueue& npcAiInputQueue);
    WorldFrameApplier(Field& field, MovementIntentQueue& movementQueue,
                      NpcAiInputQueue& npcAiInputQueue);

    bool apply(const FrameActions& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);
    bool apply(const WorldFrameInputs& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);

private:
    Field& field;
    MovementIntentQueue* movementQueue;
    NpcAiInputQueue* npcAiInputQueue;
    WorldFrameUpdateBuilder updateBuilder;
};

}

#endif

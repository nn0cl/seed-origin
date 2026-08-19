#ifndef SEED_WORLD_FRAME_APPLIER_H
#define SEED_WORLD_FRAME_APPLIER_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "ClientInputSequence.h"
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
    bool capturePublicSnapshot(uint64_t worldTick,
                               std::vector<network::WorldUpdate>& updates,
                               std::string& error);
    bool capturePublicSnapshotIfNewSessions(
        size_t newAuthenticatedSessions, uint64_t worldTick,
        std::vector<network::WorldUpdate>& updates,
        std::vector<MovementAck>& publishAcks, std::string& error);
    const std::vector<MovementAck>& ownerMovementAcks() const;
    const std::vector<MovementAck>& snapshotLocalAcks() const;

private:
    Field& field;
    MovementIntentQueue* movementQueue;
    NpcAiInputQueue* npcAiInputQueue;
    WorldFrameUpdateBuilder updateBuilder;
    std::vector<MovementAck> ownerAcks;
    std::vector<MovementAck> snapshotAcks;
    std::map<int64_t, uint64_t> lastProcessedBySession;

    void recordOwnerAck(int64_t sessionId, uint64_t worldTick,
                        uint64_t lastProcessed);
};

}

#endif

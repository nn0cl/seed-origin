#ifndef SEED_WORLD_FRAME_UPDATE_BUILDER_H
#define SEED_WORLD_FRAME_UPDATE_BUILDER_H

#include <cstdint>
#include <string>
#include <vector>

#include "ServerTick.h"
#include "ClientInputSequence.h"
#include "WorldInputTick.h"
#include "WorldUpdate.h"

namespace server {

struct CombatResolution;

class WorldFrameUpdateBuilder {
public:
    WorldFrameUpdateBuilder();

    bool build(const FrameActions& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);
    bool build(const WorldFrameInputs& frame,
               std::vector<network::WorldUpdate>& updates,
               std::string& error);
    bool appendHazard(uint64_t worldTick, float severity, float instability,
                      std::vector<network::WorldUpdate>& updates,
                      std::string& error);
    bool appendCombatResolution(uint64_t worldTick,
                                const CombatResolution& resolution,
                                std::vector<network::WorldUpdate>& updates,
                                std::string& error);
    bool appendSnapshot(uint64_t worldTick, const std::string& payload,
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

// Same WorldUpdate.sequence as the public Event. Owner copies may carry
// lastProcessedInputSequence and colon-delimited absolute pose. Other
// sessions may receive public x= y= z= pose and must not receive owner ack
// fields.
[[nodiscard]] bool copyWorldUpdateForSession(
    const network::WorldUpdate& publicUpdate, int64_t recipientSessionId,
    const std::vector<MovementAck>& ownerAcks,
    network::WorldUpdate& personalized, std::string& error);

}

#endif

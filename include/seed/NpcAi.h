#ifndef SEED_NPC_AI_H
#define SEED_NPC_AI_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

#include "Position.h"

namespace server {

struct NpcAiObservation {
    int64_t npcId;
    bool npcAlive;
    Position npcPosition;
    Position targetPosition;
    float step;
};

struct NpcAiDecision {
    int64_t npcId;
    float dx;
    float dy;
    float dz;
};

struct NpcAiInput {
    uint64_t sequence;
    NpcAiDecision decision;
};

class NpcAiPlanner {
public:
    static bool plan(const NpcAiObservation& observation,
                     NpcAiDecision& decision);
};

class NpcAiInputQueue {
public:
    inline static constexpr std::size_t MAX_PENDING_INPUTS = 4096;

    NpcAiInputQueue();
    bool enqueue(const NpcAiDecision& decision);
    std::vector<NpcAiInput> takeFrame();
    bool restoreFrame(const std::vector<NpcAiInput>& frame);
    std::size_t pendingCount() const;

private:
    std::deque<NpcAiInput> pending;
    uint64_t nextSequence;
};

}

#endif

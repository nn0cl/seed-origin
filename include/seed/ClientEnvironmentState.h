#ifndef SEED_CLIENT_ENVIRONMENT_STATE_H
#define SEED_CLIENT_ENVIRONMENT_STATE_H

#include <cstdint>
#include <vector>

#include "NpcSnapshot.h"
#include "PlayerPoseSnapshot.h"

namespace client {

struct EnvironmentState {
    float fire;
    float water;
    float earth;
    float air;
    float hazard;
    uint64_t worldTick;
    uint64_t sequence;
    std::vector<NpcSnapshot> npcs;
    std::vector<PlayerPoseSnapshot> players;
    bool hasLocalPlayer;
    float localX;
    float localY;
    float localZ;
    uint64_t lastProcessedInputSequence;
};

class ClientEnvironmentState {
public:
    ClientEnvironmentState();

    const EnvironmentState& value() const;
    void replace(const EnvironmentState& next);

private:
    EnvironmentState state;
};

}

#endif

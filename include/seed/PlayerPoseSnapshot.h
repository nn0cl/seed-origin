#ifndef SEED_PLAYER_POSE_SNAPSHOT_H
#define SEED_PLAYER_POSE_SNAPSHOT_H

#include <cstdint>

struct PlayerPoseSnapshot {
    std::int64_t sessionId;
    float x;
    float y;
    float z;
};

#endif

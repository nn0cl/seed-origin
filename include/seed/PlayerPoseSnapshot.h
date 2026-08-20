#ifndef SEED_PLAYER_POSE_SNAPSHOT_H
#define SEED_PLAYER_POSE_SNAPSHOT_H

#include <cstdint>
#include <string>

struct PlayerPoseSnapshot {
    std::int64_t sessionId = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    std::int64_t gameplayId = 0;
    std::string name;
};

#endif

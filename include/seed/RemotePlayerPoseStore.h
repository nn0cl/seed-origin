#ifndef SEED_REMOTE_PLAYER_POSE_STORE_H
#define SEED_REMOTE_PLAYER_POSE_STORE_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "LocalMovementPredictor.h"
#include "PlayerPoseSnapshot.h"

namespace client {

struct RemotePlayerPose {
    int64_t gameplayId;
    int64_t sessionId;
    std::string name;
    PredictedPose authority;
    PredictedPose rendered;
};

class RemotePlayerPoseStore {
public:
    RemotePlayerPoseStore();

    void replaceFromSnapshot(const std::vector<PlayerPoseSnapshot>& poses,
                             int64_t localSessionId);
    bool applyAuthoritative(int64_t sessionId, float x, float y, float z,
                            bool snapBaseline);
    void stepRender(float dtSeconds);
    const RemotePlayerPose* find(int64_t gameplayId) const;
    std::size_t count() const;
    void clear();

private:
    std::map<int64_t, RemotePlayerPose> poses;
    std::map<int64_t, int64_t> sessionToGameplayId;
    std::map<int64_t, float> correctionSeconds;

    void correctRender(RemotePlayerPose& pose, float& remaining,
                       float errorDistance, bool snap);
};

}

#endif

#include "RemotePlayerPoseStore.h"

#include <cmath>

namespace client {
namespace {

float poseDistance(const PredictedPose& a, const PredictedPose& b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    const float dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

}

RemotePlayerPoseStore::RemotePlayerPoseStore() : poses(), correctionSeconds() {}

void RemotePlayerPoseStore::correctRender(RemotePlayerPose& pose,
                                          float& remaining, float errorDistance,
                                          bool snap) {
    if (snap || errorDistance >= PREDICTION_SNAP_DISTANCE) {
        pose.rendered = pose.authority;
        remaining = 0.0f;
        return;
    }
    if (errorDistance > 0.0001f) {
        remaining = PREDICTION_CORRECTION_SECONDS;
        return;
    }
    pose.rendered = pose.authority;
    remaining = 0.0f;
}

void RemotePlayerPoseStore::replaceFromSnapshot(
    const std::vector<PlayerPoseSnapshot>& publicPoses, int64_t localSessionId) {
    std::map<int64_t, RemotePlayerPose> next;
    for (std::size_t i = 0; i < publicPoses.size(); ++i) {
        const PlayerPoseSnapshot& snapshot = publicPoses[i];
        if (snapshot.sessionId <= 0) continue;
        if (localSessionId > 0 && snapshot.sessionId == localSessionId) continue;
        RemotePlayerPose pose;
        pose.sessionId = snapshot.sessionId;
        pose.authority = {snapshot.x, snapshot.y, snapshot.z};
        pose.rendered = pose.authority;
        next[snapshot.sessionId] = pose;
    }
    poses.swap(next);
    correctionSeconds.clear();
}

bool RemotePlayerPoseStore::applyAuthoritative(int64_t sessionId, float x,
                                               float y, float z,
                                               bool snapBaseline) {
    if (sessionId <= 0 || !std::isfinite(x) || !std::isfinite(y) ||
        !std::isfinite(z)) {
        return false;
    }
    std::map<int64_t, RemotePlayerPose>::iterator found = poses.find(sessionId);
    if (found == poses.end()) {
        RemotePlayerPose pose;
        pose.sessionId = sessionId;
        pose.authority = {x, y, z};
        pose.rendered = pose.authority;
        poses[sessionId] = pose;
        correctionSeconds[sessionId] = 0.0f;
        return true;
    }
    found->second.authority = {x, y, z};
    const float error = poseDistance(found->second.rendered, found->second.authority);
    correctRender(found->second, correctionSeconds[sessionId], error, snapBaseline);
    return true;
}

void RemotePlayerPoseStore::stepRender(float dtSeconds) {
    if (dtSeconds < 0.0f) dtSeconds = 0.0f;
    for (std::map<int64_t, RemotePlayerPose>::iterator it = poses.begin();
         it != poses.end(); ++it) {
        float& remaining = correctionSeconds[it->first];
        if (remaining <= 0.0f) {
            it->second.rendered = it->second.authority;
            continue;
        }
        const float t = dtSeconds >= remaining ? 1.0f : dtSeconds / remaining;
        PredictedPose& rendered = it->second.rendered;
        const PredictedPose& authority = it->second.authority;
        rendered.x += (authority.x - rendered.x) * t;
        rendered.y += (authority.y - rendered.y) * t;
        rendered.z += (authority.z - rendered.z) * t;
        remaining -= dtSeconds;
        if (remaining <= 0.0f) {
            remaining = 0.0f;
            rendered = authority;
        }
    }
}

const RemotePlayerPose* RemotePlayerPoseStore::find(int64_t sessionId) const {
    std::map<int64_t, RemotePlayerPose>::const_iterator found = poses.find(sessionId);
    return found == poses.end() ? 0 : &found->second;
}

std::size_t RemotePlayerPoseStore::count() const { return poses.size(); }

void RemotePlayerPoseStore::clear() {
    poses.clear();
    correctionSeconds.clear();
}

}

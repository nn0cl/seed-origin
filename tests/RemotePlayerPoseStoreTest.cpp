#include <cassert>
#include <cmath>
#include <string>
#include <vector>

#include "RemotePlayerPoseStore.h"

namespace remote_player_pose_store_tests {
namespace {

bool nearly(float actual, float expected) {
    return std::fabs(actual - expected) < 0.0001f;
}

PlayerPoseSnapshot publicPose(int64_t sessionId, int64_t gameplayId, float x,
                              float y, float z, const std::string& name) {
    PlayerPoseSnapshot pose;
    pose.sessionId = sessionId;
    pose.gameplayId = gameplayId;
    pose.x = x;
    pose.y = y;
    pose.z = z;
    pose.name = name;
    return pose;
}

}

void snaps_on_snapshot_replace_and_skips_local_session() {
    client::RemotePlayerPoseStore store;
    std::vector<PlayerPoseSnapshot> poses;
    poses.push_back(publicPose(10, 1, 3.0f, 0.0f, 0.0f, "Local"));
    poses.push_back(publicPose(99, 7, 1.0f, 2.0f, 3.0f, "Hero"));
    store.replaceFromSnapshot(poses, 10);
    assert(store.find(1) == 0);
    assert(store.find(99) == 0);
    const client::RemotePlayerPose* remote = store.find(7);
    assert(remote != 0);
    assert(remote->gameplayId == 7);
    assert(remote->sessionId == 99);
    assert(remote->name == "Hero");
    assert(nearly(remote->authority.x, 1.0f));
    assert(nearly(remote->rendered.x, 1.0f));
    assert(store.count() == 1);
}

void interpolates_small_error_and_snaps_large_error() {
    client::RemotePlayerPoseStore store;
    std::vector<PlayerPoseSnapshot> poses;
    poses.push_back(publicPose(99, 7, 0.0f, 0.0f, 0.0f, "Hero"));
    store.replaceFromSnapshot(poses, 10);
    assert(store.applyAuthoritative(99, 0.2f, 0.0f, 0.0f, false));
    const float before = store.find(7)->rendered.x;
    assert(std::fabs(before - store.find(7)->authority.x) > 0.0001f);
    store.stepRender(0.15f);
    assert(std::fabs(store.find(7)->rendered.x - store.find(7)->authority.x) <
           0.05f);

    client::RemotePlayerPoseStore snap;
    std::vector<PlayerPoseSnapshot> snapPoses;
    snapPoses.push_back(publicPose(8, 3, 0.0f, 0.0f, 0.0f, ""));
    snap.replaceFromSnapshot(snapPoses, 10);
    assert(snap.applyAuthoritative(8, 80.0f, 0.0f, 0.0f, false));
    assert(nearly(snap.find(3)->rendered.x, 80.0f));
    assert(nearly(snap.find(3)->authority.x, 80.0f));
}

void reconnect_same_gameplay_id_is_one_remote_and_updates_name() {
    client::RemotePlayerPoseStore store;
    std::vector<PlayerPoseSnapshot> first;
    first.push_back(publicPose(99, 7, 1.0f, 0.0f, 0.0f, "Hero"));
    store.replaceFromSnapshot(first, 10);
    assert(store.count() == 1);
    assert(store.find(7)->sessionId == 99);

    std::vector<PlayerPoseSnapshot> rebound;
    rebound.push_back(publicPose(100, 7, 4.0f, 0.0f, 0.0f, "Hero"));
    store.replaceFromSnapshot(rebound, 10);
    assert(store.count() == 1);
    const client::RemotePlayerPose* remote = store.find(7);
    assert(remote != 0);
    assert(remote->sessionId == 100);
    assert(remote->name == "Hero");
    assert(nearly(remote->authority.x, 4.0f));
    assert(nearly(remote->rendered.x, 4.0f));
    assert(store.find(99) == 0);
    assert(!store.applyAuthoritative(99, 9.0f, 0.0f, 0.0f, false));
    assert(store.applyAuthoritative(100, 5.0f, 0.0f, 0.0f, true));
    assert(nearly(store.find(7)->authority.x, 5.0f));
}

void unknown_session_does_not_invent_a_remote() {
    client::RemotePlayerPoseStore store;
    assert(!store.applyAuthoritative(99, 1.0f, 0.0f, 0.0f, true));
    assert(store.count() == 0);
    std::vector<PlayerPoseSnapshot> withoutId;
    PlayerPoseSnapshot fixture;
    fixture.sessionId = 99;
    fixture.x = 1.0f;
    withoutId.push_back(fixture);
    store.replaceFromSnapshot(withoutId, 10);
    assert(store.count() == 0);
}

} // namespace remote_player_pose_store_tests

#include <cassert>
#include <cmath>

#include "RemotePlayerPoseStore.h"

namespace remote_player_pose_store_tests {
namespace {

bool nearly(float actual, float expected) {
    return std::fabs(actual - expected) < 0.0001f;
}

}

void snaps_on_snapshot_replace_and_skips_local_session() {
    client::RemotePlayerPoseStore store;
    std::vector<PlayerPoseSnapshot> poses;
    poses.push_back({10, 3.0f, 0.0f, 0.0f});
    poses.push_back({99, 1.0f, 2.0f, 3.0f});
    store.replaceFromSnapshot(poses, 10);
    assert(store.find(10) == 0);
    const client::RemotePlayerPose* remote = store.find(99);
    assert(remote != 0);
    assert(nearly(remote->authority.x, 1.0f));
    assert(nearly(remote->rendered.x, 1.0f));
    assert(store.count() == 1);
}

void interpolates_small_error_and_snaps_large_error() {
    client::RemotePlayerPoseStore store;
    assert(store.applyAuthoritative(99, 0.0f, 0.0f, 0.0f, true));
    assert(store.applyAuthoritative(99, 0.2f, 0.0f, 0.0f, false));
    const float before = store.find(99)->rendered.x;
    assert(std::fabs(before - store.find(99)->authority.x) > 0.0001f);
    store.stepRender(0.15f);
    assert(std::fabs(store.find(99)->rendered.x - store.find(99)->authority.x) <
           0.05f);

    client::RemotePlayerPoseStore snap;
    assert(snap.applyAuthoritative(7, 0.0f, 0.0f, 0.0f, true));
    assert(snap.applyAuthoritative(7, 80.0f, 0.0f, 0.0f, false));
    assert(nearly(snap.find(7)->rendered.x, 80.0f));
    assert(nearly(snap.find(7)->authority.x, 80.0f));
}

} // namespace remote_player_pose_store_tests

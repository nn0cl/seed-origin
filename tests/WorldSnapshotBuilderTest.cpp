#include <cassert>

#include "WorldSnapshotBuilder.h"

namespace world_snapshot_builder_tests {

void builds_environment_snapshot() {
    world::EnvironmentEther ether;
    assert(ether.add(world::EtherAttribute::Fire, 25.0f));
    server::WorldSnapshotBuilder builder;
    network::WorldUpdate snapshot = {};
    std::string error;
    assert(builder.build(7, ether, 0.5f, snapshot, error));
    assert(snapshot.kind == network::UpdateKind::Snapshot);
    assert(snapshot.worldTick == 7);
    assert(snapshot.eventId == 0);
    assert(snapshot.payload.find("ether.fire=") == 0);
    assert(network::validateWorldUpdate(snapshot, error));
}

void rejects_negative_hazard() {
    world::EnvironmentEther ether;
    server::WorldSnapshotBuilder builder;
    network::WorldUpdate snapshot = {};
    std::string error;
    assert(!builder.build(1, ether, -1.0f, snapshot, error));
}

void publishes_only_public_live_npc_state() {
    world::EnvironmentEther ether;
    server::WorldSnapshotBuilder builder;
    network::WorldUpdate snapshot = {};
    const std::vector<NpcSnapshot> npcs = {
        {19510, "goblin", 1.0f, 2.0f, 3.0f, 40, true}};
    std::string error;
    assert(builder.build(2, ether, 0.0f, npcs, snapshot, error));
    assert(snapshot.payload.find("npc.count=1") != std::string::npos);
    assert(snapshot.payload.find("npc.0.type=goblin") != std::string::npos);
}

void publishes_public_player_poses_without_owner_ack() {
    world::EnvironmentEther ether;
    server::WorldSnapshotBuilder builder;
    network::WorldUpdate snapshot = {};
    PlayerPoseSnapshot first;
    first.sessionId = 10;
    first.x = 1.0f;
    first.y = 2.0f;
    first.z = 3.0f;
    first.name = "Alpha";
    PlayerPoseSnapshot second;
    second.sessionId = 20;
    second.x = 4.0f;
    second.y = 5.0f;
    second.z = 6.0f;
    second.name = "Beta";
    const std::vector<PlayerPoseSnapshot> players = {first, second};
    std::string error;
    assert(builder.build(3, ether, 0.0f, std::vector<NpcSnapshot>(), players,
                         snapshot, error));
    assert(snapshot.payload.find("player.count=2") != std::string::npos);
    assert(snapshot.payload.find("player.0.session=10") != std::string::npos);
    assert(snapshot.payload.find("player.0.name=Alpha") != std::string::npos);
    assert(snapshot.payload.find("player.1.session=20") != std::string::npos);
    assert(snapshot.payload.find("player.1.name=Beta") != std::string::npos);
    assert(snapshot.payload.find("lastProcessedInputSequence") ==
           std::string::npos);
    assert(snapshot.payload.find("local.") == std::string::npos);
}

void rejects_empty_player_name_on_snapshot() {
    world::EnvironmentEther ether;
    server::WorldSnapshotBuilder builder;
    network::WorldUpdate snapshot = {};
    PlayerPoseSnapshot unnamed;
    unnamed.sessionId = 10;
    unnamed.x = 1.0f;
    unnamed.name = "";
    std::string error;
    assert(!builder.build(3, ether, 0.0f, std::vector<NpcSnapshot>(),
                          std::vector<PlayerPoseSnapshot>{unnamed}, snapshot,
                          error));
}

} // namespace world_snapshot_builder_tests

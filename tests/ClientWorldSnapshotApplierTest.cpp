#include <cassert>

#include "ClientWorldSnapshotApplier.h"

namespace client_snapshot_tests {

void applies_environment_snapshot_and_resets_sequence() {
    network::WorldUpdate update = {1, network::UpdateKind::Snapshot, 12, 4, 0,
                                   "ether.fire=2;ether.water=-1;ether.earth=0;ether.air=3;ether.hazard=1"};
    client::ClientEnvironmentState state;
    client::ClientWorldSnapshotApplier applier;
    std::string error;
    assert(applier.applySnapshot(update, state, error));
    assert(state.value().fire == 2.0f);
    assert(state.value().hazard == 1.0f);
    assert(state.value().worldTick == 12);
    assert(applier.synchronization().expectedSequence() == 5);
}

void rejects_incomplete_or_invalid_environment_snapshot() {
    network::WorldUpdate update = {1, network::UpdateKind::Snapshot, 1, 1, 0,
                                   "ether.fire=nan;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0"};
    client::ClientEnvironmentState state;
    client::ClientWorldSnapshotApplier applier;
    std::string error;
    assert(!applier.applySnapshot(update, state, error));
    assert(state.value().fire == 0.0f);
}

void applies_public_npc_snapshot_state() {
    network::WorldUpdate update = {
        1, network::UpdateKind::Snapshot, 2, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "npc.count=1;npc.0.id=19510;npc.0.type=goblin;npc.0.x=1;"
        "npc.0.y=2;npc.0.z=3;npc.0.hp=40;npc.0.alive=1"};
    client::ClientEnvironmentState state;
    client::ClientWorldSnapshotApplier applier;
    std::string error;
    assert(applier.applySnapshot(update, state, error));
    assert(state.value().npcs.size() == 1);
    assert(state.value().npcs[0].id == 19510);
}

void applies_public_player_snapshot_poses() {
    network::WorldUpdate update = {
        1, network::UpdateKind::Snapshot, 2, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "player.count=1;player.0.session=99;player.0.x=8;player.0.y=0;"
        "player.0.z=0"};
    client::ClientEnvironmentState state;
    client::ClientWorldSnapshotApplier applier;
    std::string error;
    assert(applier.applySnapshot(update, state, error));
    assert(state.value().players.size() == 1);
    assert(state.value().players[0].sessionId == 99);
    assert(state.value().players[0].x == 8.0f);
    assert(state.value().players[0].gameplayId == 0);
    assert(state.value().players[0].name.empty());
    assert(!state.value().hasLocalPlayer);
}

void applies_gameplay_id_and_display_name_without_auth_id() {
    network::WorldUpdate update = {
        1, network::UpdateKind::Snapshot, 2, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "player.count=1;player.0.session=99;player.0.x=8;player.0.y=0;"
        "player.0.z=0;player.0.id=7;player.0.name=Hero"};
    client::ClientEnvironmentState state;
    client::ClientWorldSnapshotApplier applier;
    std::string error;
    assert(applier.applySnapshot(update, state, error));
    assert(state.value().players.size() == 1);
    assert(state.value().players[0].sessionId == 99);
    assert(state.value().players[0].gameplayId == 7);
    assert(state.value().players[0].name == "Hero");
    assert(update.payload.find("auth") == std::string::npos);
}

void rejects_auth_player_id_on_public_snapshot() {
    network::WorldUpdate update = {
        1, network::UpdateKind::Snapshot, 2, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "player.count=1;player.0.session=99;player.0.x=8;player.0.y=0;"
        "player.0.z=0;player.0.id=7;player.0.authPlayerId=501"};
    client::ClientEnvironmentState state;
    client::ClientWorldSnapshotApplier applier;
    std::string error;
    assert(!applier.applySnapshot(update, state, error));
    assert(state.value().players.empty());
}

} // namespace client_snapshot_tests

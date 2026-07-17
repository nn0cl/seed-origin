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

} // namespace client_snapshot_tests

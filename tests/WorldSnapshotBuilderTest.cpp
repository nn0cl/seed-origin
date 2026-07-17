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

} // namespace world_snapshot_builder_tests

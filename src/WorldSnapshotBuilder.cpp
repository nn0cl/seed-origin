#include "WorldSnapshotBuilder.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace server {

WorldSnapshotBuilder::WorldSnapshotBuilder() : sequence(1) {}

bool WorldSnapshotBuilder::build(uint64_t worldTick,
                                 const world::EnvironmentEther& ether,
                                 float hazard,
                                 network::WorldUpdate& snapshot,
                                 std::string& error) {
    if (!std::isfinite(hazard) || hazard < 0.0f) {
        error = "snapshot hazard is invalid";
        return false;
    }
    if (sequence == std::numeric_limits<uint64_t>::max()) {
        error = "snapshot sequence exhausted";
        return false;
    }
    std::ostringstream payload;
    payload << "ether.fire=" << ether.value(world::EtherAttribute::Fire)
            << ";ether.water=" << ether.value(world::EtherAttribute::Water)
            << ";ether.earth=" << ether.value(world::EtherAttribute::Earth)
            << ";ether.air=" << ether.value(world::EtherAttribute::Air)
            << ";ether.hazard=" << hazard;
    snapshot = {
        network::CURRENT_PROTOCOL_VERSION,
        network::UpdateKind::Snapshot,
        worldTick,
        sequence++,
        0,
        payload.str()
    };
    if (!network::validateWorldUpdate(snapshot, error)) return false;
    error.clear();
    return true;
}

uint64_t WorldSnapshotBuilder::nextSequence() const {
    return sequence;
}

}

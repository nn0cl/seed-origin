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
    return build(worldTick, ether, hazard, std::vector<NpcSnapshot>(),
                 snapshot, error);
}

bool WorldSnapshotBuilder::build(uint64_t worldTick,
                                 const world::EnvironmentEther& ether,
                                 float hazard,
                                 const std::vector<NpcSnapshot>& npcs,
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
            << ";ether.hazard=" << hazard
            << ";npc.count=" << npcs.size();
    for (std::size_t index = 0; index < npcs.size(); ++index) {
        const NpcSnapshot& npc = npcs[index];
        if (npc.id <= 0 || npc.type.empty() || npc.type.size() > 32 ||
            !std::isfinite(npc.x) || !std::isfinite(npc.y) ||
            !std::isfinite(npc.z) || npc.hp < 0 || !npc.alive) {
            error = "public NPC snapshot contains invalid state";
            return false;
        }
        payload << ";npc." << index << ".id=" << npc.id
                << ";npc." << index << ".type=" << npc.type
                << ";npc." << index << ".x=" << npc.x
                << ";npc." << index << ".y=" << npc.y
                << ";npc." << index << ".z=" << npc.z
                << ";npc." << index << ".hp=" << npc.hp
                << ";npc." << index << ".alive=1";
    }
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

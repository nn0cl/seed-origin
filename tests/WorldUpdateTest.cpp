#include <cassert>

#include "WorldUpdate.h"

namespace world_update_tests {

void accepts_snapshot_and_event_envelopes() {
    const network::WorldUpdate snapshot = {
        network::CURRENT_PROTOCOL_VERSION,
        network::UpdateKind::Snapshot,
        10,
        1,
        0,
        "state"
    };
    const network::WorldUpdate event = {
        network::CURRENT_PROTOCOL_VERSION,
        network::UpdateKind::Event,
        10,
        2,
        11,
        "spell-hit"
    };
    std::string error;
    assert(network::validateWorldUpdate(snapshot, error));
    assert(network::validateWorldUpdate(event, error));
}

void rejects_invalid_update_identity() {
    const network::WorldUpdate invalidEvent = {
        network::CURRENT_PROTOCOL_VERSION,
        network::UpdateKind::Event,
        10,
        2,
        0,
        "spell-hit"
    };
    std::string error;
    assert(!network::validateWorldUpdate(invalidEvent, error));
}

} // namespace world_update_tests

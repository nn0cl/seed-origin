#include <cassert>

#include "WorldUpdateSync.h"

namespace world_update_sync_tests {

void requests_snapshot_on_gap_and_does_not_apply_it() {
    network::WorldUpdateSyncController controller;
    const network::WorldUpdate first = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Snapshot, 1, 1, 0, "state"};
    const network::WorldUpdate gap = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Event, 1, 3, 4, "event"};

    assert(controller.receive(first).decision == network::SyncDecision::Apply);
    const network::SyncResult result = controller.receive(gap);
    assert(result.decision == network::SyncDecision::RequestSnapshot);
    assert(controller.expectedSequence() == 2);
}

void resumes_after_snapshot_confirmation() {
    network::WorldUpdateSyncController controller;
    controller.confirmSnapshot(10);
    const network::WorldUpdate event = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Event, 5, 11, 12, "event"};
    assert(controller.receive(event).decision == network::SyncDecision::Apply);
}

} // namespace world_update_sync_tests

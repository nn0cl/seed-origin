#include <cassert>

#include "WorldUpdateSequence.h"

namespace world_update_sequence_tests {

void accepts_ordered_updates_and_initial_snapshot() {
    network::WorldUpdateSequenceTracker tracker;
    std::string error;
    const network::WorldUpdate snapshot = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Snapshot, 1, 10, 0, "state"};
    const network::WorldUpdate event = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Event, 1, 11, 20, "event"};

    assert(tracker.accept(snapshot, error) == network::UpdateSequenceStatus::Accepted);
    assert(tracker.accept(event, error) == network::UpdateSequenceStatus::Accepted);
    assert(tracker.nextSequence() == 12);
}

void detects_duplicates_and_gaps_without_advancing() {
    network::WorldUpdateSequenceTracker tracker;
    std::string error;
    const network::WorldUpdate first = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Snapshot, 1, 1, 0, "state"};
    const network::WorldUpdate gap = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Event, 1, 3, 4, "event"};

    assert(tracker.accept(first, error) == network::UpdateSequenceStatus::Accepted);
    assert(tracker.accept(first, error) == network::UpdateSequenceStatus::Duplicate);
    assert(tracker.accept(gap, error) == network::UpdateSequenceStatus::Gap);
    assert(tracker.nextSequence() == 2);
}

} // namespace world_update_sequence_tests

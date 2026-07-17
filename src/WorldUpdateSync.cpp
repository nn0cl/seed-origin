#include "WorldUpdateSync.h"

namespace network {

WorldUpdateSyncController::WorldUpdateSyncController() : tracker() {}

SyncResult WorldUpdateSyncController::receive(const WorldUpdate& update) {
    std::string error;
    const UpdateSequenceStatus status = tracker.accept(update, error);
    SyncDecision decision = SyncDecision::Reject;
    if (status == UpdateSequenceStatus::Accepted) {
        decision = SyncDecision::Apply;
    } else if (status == UpdateSequenceStatus::Duplicate) {
        decision = SyncDecision::IgnoreDuplicate;
    } else if (status == UpdateSequenceStatus::Gap) {
        decision = SyncDecision::RequestSnapshot;
    }
    return {decision, tracker.nextSequence(), error};
}

void WorldUpdateSyncController::confirmSnapshot(uint64_t sequence) {
    tracker.resetAfterSnapshot(sequence);
}

uint64_t WorldUpdateSyncController::expectedSequence() const {
    return tracker.nextSequence();
}

}

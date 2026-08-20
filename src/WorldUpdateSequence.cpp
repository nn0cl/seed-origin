#include "WorldUpdateSequence.h"

#include <limits>

namespace network {

WorldUpdateSequenceTracker::WorldUpdateSequenceTracker()
    : hasSequence(false), expectedSequence(1) {}

UpdateSequenceStatus WorldUpdateSequenceTracker::accept(
    const WorldUpdate& update, std::string& error) {
    if (!validateWorldUpdate(update, error)) return UpdateSequenceStatus::Invalid;
    if (update.sequence == std::numeric_limits<uint64_t>::max()) {
        error = "world update sequence cannot be incremented";
        return UpdateSequenceStatus::Invalid;
    }

    if (!hasSequence) {
        // Join/resync Snapshots may start the stream at any sequence.
        // An Event before that baseline must still be the expected next
        // sequence (1); a jump is a gap, not a bootstrap.
        if (update.kind == UpdateKind::Event &&
            update.sequence != expectedSequence) {
            if (update.sequence > expectedSequence) {
                error = "world update sequence gap detected";
                return UpdateSequenceStatus::Gap;
            }
            error = "world update sequence is a duplicate or stale";
            return UpdateSequenceStatus::Duplicate;
        }
        hasSequence = true;
        expectedSequence = update.sequence + 1;
        return UpdateSequenceStatus::Accepted;
    }
    if (update.sequence < expectedSequence) {
        error = "world update sequence is a duplicate or stale";
        return UpdateSequenceStatus::Duplicate;
    }
    if (update.sequence > expectedSequence) {
        error = "world update sequence gap detected";
        return UpdateSequenceStatus::Gap;
    }
    ++expectedSequence;
    error.clear();
    return UpdateSequenceStatus::Accepted;
}

void WorldUpdateSequenceTracker::resetAfterSnapshot(uint64_t sequence) {
    hasSequence = true;
    expectedSequence = sequence + 1;
}

bool WorldUpdateSequenceTracker::initialized() const {
    return hasSequence;
}

uint64_t WorldUpdateSequenceTracker::nextSequence() const {
    return expectedSequence;
}

}

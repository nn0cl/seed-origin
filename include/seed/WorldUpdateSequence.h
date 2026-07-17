#ifndef SEED_WORLD_UPDATE_SEQUENCE_H
#define SEED_WORLD_UPDATE_SEQUENCE_H

#include <cstdint>
#include <string>

#include "WorldUpdate.h"

namespace network {

enum class UpdateSequenceStatus {
    Accepted,
    Duplicate,
    Gap,
    Invalid
};

class WorldUpdateSequenceTracker {
public:
    WorldUpdateSequenceTracker();

    UpdateSequenceStatus accept(const WorldUpdate& update, std::string& error);
    void resetAfterSnapshot(uint64_t sequence);
    bool initialized() const;
    uint64_t nextSequence() const;

private:
    bool hasSequence;
    uint64_t expectedSequence;
};

}

#endif

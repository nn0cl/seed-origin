#ifndef SEED_WORLD_UPDATE_ACCUMULATOR_H
#define SEED_WORLD_UPDATE_ACCUMULATOR_H

#include <stdint.h>
#include <string>
#include <vector>

#include "WorldUpdate.h"

namespace network {

class WorldUpdateAccumulator {
public:
    WorldUpdateAccumulator();
    bool append(const std::vector<uint8_t>& bytes,
                std::vector<WorldUpdate>& updates, std::string& error);
    void reset();

private:
    std::vector<uint8_t> buffered;
    bool failed;
};

}

#endif

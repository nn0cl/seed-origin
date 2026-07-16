#ifndef SEED_FRAME_ACCUMULATOR_H
#define SEED_FRAME_ACCUMULATOR_H

#include <stdint.h>
#include <string>
#include <vector>

#include "NetworkCommand.h"

namespace network {

class FrameAccumulator {
public:
    FrameAccumulator();
    bool append(const std::vector<uint8_t>& bytes,
                std::vector<NetworkCommand>& commands,
                std::string& error);
    void reset();

private:
    std::vector<uint8_t> buffered;
    bool failed;
};

}

#endif

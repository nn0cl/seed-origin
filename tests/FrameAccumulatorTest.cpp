#include <cassert>

#include "FrameAccumulator.h"
#include "NetworkFrameCodec.h"

namespace frame_accumulator_tests {

void joins_partial_frame_and_preserves_multiple_frames() {
    network::NetworkCommand first = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Chat, 1, "one"
    };
    network::NetworkCommand second = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Chat, 1, "two"
    };
    std::vector<uint8_t> firstFrame;
    std::vector<uint8_t> secondFrame;
    std::string error;
    assert(network::encodeFrame(first, firstFrame, error));
    assert(network::encodeFrame(second, secondFrame, error));

    network::FrameAccumulator accumulator;
    std::vector<network::NetworkCommand> commands;
    const std::vector<uint8_t> firstPart(firstFrame.begin(), firstFrame.begin() + 5);
    assert(accumulator.append(firstPart, commands, error));
    assert(commands.empty());

    std::vector<uint8_t> remainder(firstFrame.begin() + 5, firstFrame.end());
    remainder.insert(remainder.end(), secondFrame.begin(), secondFrame.end());
    assert(accumulator.append(remainder, commands, error));
    assert(commands.size() == 2);
    assert(commands[0].payload == "one");
    assert(commands[1].payload == "two");
}

} // namespace frame_accumulator_tests

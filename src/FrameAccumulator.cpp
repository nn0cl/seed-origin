#include "FrameAccumulator.h"

#include "NetworkFrameCodec.h"

namespace network {

FrameAccumulator::FrameAccumulator() : failed(false) {}

bool FrameAccumulator::append(const std::vector<uint8_t>& bytes,
                              std::vector<NetworkCommand>& commands,
                              std::string& error) {
    commands.clear();
    if (failed) {
        error = "frame accumulator is failed";
        return false;
    }
    if (bytes.size() > FRAME_HEADER_SIZE + MAX_COMMAND_PAYLOAD - buffered.size()) {
        failed = true;
        error = "stream frame exceeds maximum size";
        return false;
    }
    buffered.insert(buffered.end(), bytes.begin(), bytes.end());

    while (buffered.size() >= FRAME_HEADER_SIZE) {
        const uint32_t payloadLength =
            (static_cast<uint32_t>(buffered[12]) << 24) |
            (static_cast<uint32_t>(buffered[13]) << 16) |
            (static_cast<uint32_t>(buffered[14]) << 8) |
            static_cast<uint32_t>(buffered[15]);
        if (payloadLength > MAX_COMMAND_PAYLOAD) {
            failed = true;
            error = "stream payload exceeds maximum size";
            return false;
        }
        const size_t frameSize = FRAME_HEADER_SIZE + payloadLength;
        if (buffered.size() < frameSize) return true;

        const std::vector<uint8_t> complete(buffered.begin(),
                                             buffered.begin() + frameSize);
        buffered.erase(buffered.begin(), buffered.begin() + frameSize);
        NetworkCommand command = {};
        if (!decodeFrame(complete, command, error)) {
            failed = true;
            return false;
        }
        commands.push_back(command);
    }
    return true;
}

void FrameAccumulator::reset() {
    buffered.clear();
    failed = false;
}

}

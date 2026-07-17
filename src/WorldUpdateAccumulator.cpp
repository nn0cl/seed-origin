#include "WorldUpdateAccumulator.h"

#include "WorldUpdateFrameCodec.h"

namespace network {

WorldUpdateAccumulator::WorldUpdateAccumulator() : buffered(), failed(false) {}

bool WorldUpdateAccumulator::append(const std::vector<uint8_t>& bytes,
                                    std::vector<WorldUpdate>& updates,
                                    std::string& error) {
    updates.clear();
    if (failed) {
        error = "world update accumulator is failed";
        return false;
    }
    if (bytes.size() > WORLD_UPDATE_FRAME_HEADER_SIZE + MAX_UPDATE_PAYLOAD -
                          buffered.size()) {
        failed = true;
        error = "world update stream frame exceeds maximum size";
        return false;
    }
    buffered.insert(buffered.end(), bytes.begin(), bytes.end());
    while (buffered.size() >= WORLD_UPDATE_FRAME_HEADER_SIZE) {
        const uint32_t payloadLength =
            (static_cast<uint32_t>(buffered[32]) << 24) |
            (static_cast<uint32_t>(buffered[33]) << 16) |
            (static_cast<uint32_t>(buffered[34]) << 8) |
            static_cast<uint32_t>(buffered[35]);
        if (payloadLength > MAX_UPDATE_PAYLOAD) {
            failed = true;
            error = "world update stream payload exceeds maximum size";
            return false;
        }
        const size_t frameSize = WORLD_UPDATE_FRAME_HEADER_SIZE + payloadLength;
        if (buffered.size() < frameSize) return true;
        const std::vector<uint8_t> complete(buffered.begin(),
                                             buffered.begin() + frameSize);
        buffered.erase(buffered.begin(), buffered.begin() + frameSize);
        WorldUpdate update = {};
        if (!decodeWorldUpdateFrame(complete, update, error)) {
            failed = true;
            return false;
        }
        updates.push_back(update);
    }
    error.clear();
    return true;
}

void WorldUpdateAccumulator::reset() {
    buffered.clear();
    failed = false;
}

}

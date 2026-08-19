#include "ClientInboundDemux.h"

#include "NetworkFrameCodec.h"
#include "WorldUpdate.h"
#include "WorldUpdateFrameCodec.h"

namespace client {

namespace {
uint16_t read16(const std::vector<uint8_t>& input, size_t offset) {
    return static_cast<uint16_t>((input[offset] << 8) | input[offset + 1]);
}

uint32_t read32(const std::vector<uint8_t>& input, size_t offset) {
    return (static_cast<uint32_t>(input[offset]) << 24) |
           (static_cast<uint32_t>(input[offset + 1]) << 16) |
           (static_cast<uint32_t>(input[offset + 2]) << 8) |
           static_cast<uint32_t>(input[offset + 3]);
}
}

ClientInboundDemux::ClientInboundDemux() : buffered(), failedState(false) {}

bool ClientInboundDemux::append(const std::vector<uint8_t>& bytes,
                                std::vector<InboundFrame>& frames,
                                std::string& error) {
    frames.clear();
    if (failedState) {
        error = "client inbound demux is failed";
        return false;
    }
    buffered.insert(buffered.end(), bytes.begin(), bytes.end());

    while (buffered.size() >= 2) {
        const uint16_t magic = read16(buffered, 0);
        size_t headerSize = 0;
        uint32_t payloadLength = 0;
        InboundFrameKind kind = InboundFrameKind::LoginResponse;
        if (magic == network::WORLD_UPDATE_FRAME_MAGIC) {
            if (buffered.size() < network::WORLD_UPDATE_FRAME_HEADER_SIZE) {
                error.clear();
                return true;
            }
            headerSize = network::WORLD_UPDATE_FRAME_HEADER_SIZE;
            payloadLength = read32(buffered, 32);
            if (payloadLength > network::MAX_UPDATE_PAYLOAD) {
                failedState = true;
                error = "world update stream payload exceeds maximum size";
                return false;
            }
            kind = InboundFrameKind::WorldUpdate;
        } else {
            if (buffered.size() < network::FRAME_HEADER_SIZE) {
                error.clear();
                return true;
            }
            headerSize = network::FRAME_HEADER_SIZE;
            payloadLength = read32(buffered, 12);
            if (payloadLength > network::MAX_COMMAND_PAYLOAD) {
                failedState = true;
                error = "login response stream payload exceeds maximum size";
                return false;
            }
            kind = InboundFrameKind::LoginResponse;
        }
        const size_t frameSize = headerSize + payloadLength;
        if (buffered.size() < frameSize) {
            error.clear();
            return true;
        }
        InboundFrame frame;
        frame.kind = kind;
        frame.bytes.assign(buffered.begin(), buffered.begin() + frameSize);
        buffered.erase(buffered.begin(), buffered.begin() + frameSize);
        frames.push_back(frame);
    }
    error.clear();
    return true;
}

void ClientInboundDemux::reset() {
    buffered.clear();
    failedState = false;
}

bool ClientInboundDemux::failed() const {
    return failedState;
}

}

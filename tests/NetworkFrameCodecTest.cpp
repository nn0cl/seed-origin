#include <cassert>

#include "NetworkFrameCodec.h"

namespace network_frame_tests {

void round_trips_a_valid_command() {
    network::NetworkCommand original = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Chat,
        42,
        "hello"
    };
    std::vector<uint8_t> frame;
    std::string error;
    network::NetworkCommand decoded = {};
    assert(network::encodeFrame(original, frame, error));
    assert(network::decodeFrame(frame, decoded, error));
    assert(decoded.version == original.version);
    assert(decoded.type == original.type);
    assert(decoded.sessionId == original.sessionId);
    assert(decoded.payload == original.payload);
}

void rejects_incomplete_and_oversized_frames() {
    std::vector<uint8_t> frame(network::FRAME_HEADER_SIZE - 1, 0);
    network::NetworkCommand command = {};
    std::string error;
    assert(!network::decodeFrame(frame, command, error));
    frame.assign(network::FRAME_HEADER_SIZE, 0);
    frame[2] = 0;
    frame[3] = static_cast<uint8_t>(network::CommandType::Chat);
    frame[12] = 0x00;
    frame[13] = 0x00;
    frame[14] = 0x10;
    frame[15] = 0x01;
    assert(!network::decodeFrame(frame, command, error));
}

} // namespace network_frame_tests

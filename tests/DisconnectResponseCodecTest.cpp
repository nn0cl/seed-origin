#include <cassert>

#include "DisconnectResponseCodec.h"

namespace disconnect_response_codec_tests {

void round_trips_accepted_response() {
    const network::DisconnectResponse original = {
        network::CURRENT_PROTOCOL_VERSION,
        network::DisconnectResponseStatus::Accepted, 21, std::string()};
    std::vector<uint8_t> frame;
    std::string error;
    network::DisconnectResponse decoded = {};
    assert(network::encodeDisconnectResponseFrame(original, frame, error));
    assert(network::decodeDisconnectResponseFrame(frame, decoded, error));
    assert(decoded.status == original.status);
    assert(decoded.sessionId == original.sessionId);
    assert(decoded.payload.empty());
}

void round_trips_rejected_response() {
    const network::DisconnectResponse original = {
        network::CURRENT_PROTOCOL_VERSION,
        network::DisconnectResponseStatus::Rejected, 21,
        "disconnect command rate limit exceeded"};
    std::vector<uint8_t> frame;
    std::string error;
    network::DisconnectResponse decoded = {};
    assert(network::encodeDisconnectResponseFrame(original, frame, error));
    assert(network::decodeDisconnectResponseFrame(frame, decoded, error));
    assert(decoded.status == original.status);
    assert(decoded.sessionId == 21);
    assert(decoded.payload == original.payload);
}

void rejects_accepted_response_without_session() {
    const network::DisconnectResponse response = {
        network::CURRENT_PROTOCOL_VERSION,
        network::DisconnectResponseStatus::Accepted, 0, std::string()};
    std::vector<uint8_t> frame;
    std::string error;
    assert(!network::encodeDisconnectResponseFrame(response, frame, error));
}

} // namespace disconnect_response_codec_tests

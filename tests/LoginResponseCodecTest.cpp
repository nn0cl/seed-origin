#include <cassert>

#include "LoginResponseCodec.h"

namespace login_response_codec_tests {

void round_trips_accepted_response() {
    const network::LoginResponse original = {
        network::CURRENT_PROTOCOL_VERSION,
        network::LoginResponseStatus::Accepted,
        42,
        std::string()
    };
    std::vector<uint8_t> frame;
    std::string error;
    network::LoginResponse decoded = {};
    assert(network::encodeLoginResponseFrame(original, frame, error));
    assert(network::decodeLoginResponseFrame(frame, decoded, error));
    assert(decoded.status == original.status);
    assert(decoded.sessionId == original.sessionId);
}

void round_trips_rejected_response() {
    const network::LoginResponse original = {
        network::CURRENT_PROTOCOL_VERSION,
        network::LoginResponseStatus::Rejected,
        0,
        "login rejected"
    };
    std::vector<uint8_t> frame;
    std::string error;
    network::LoginResponse decoded = {};
    assert(network::encodeLoginResponseFrame(original, frame, error));
    assert(network::decodeLoginResponseFrame(frame, decoded, error));
    assert(decoded.status == original.status);
    assert(decoded.sessionId == 0);
    assert(decoded.payload == original.payload);
}

void rejects_invalid_response_identity() {
    const network::LoginResponse response = {
        network::CURRENT_PROTOCOL_VERSION,
        network::LoginResponseStatus::Accepted,
        0,
        std::string()
    };
    std::vector<uint8_t> frame;
    std::string error;
    assert(!network::encodeLoginResponseFrame(response, frame, error));
}

} // namespace login_response_codec_tests

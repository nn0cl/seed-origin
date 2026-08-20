#include "LoginResponseCodec.h"

#include "FrameHeaderBinary.h"

namespace network {

bool encodeLoginResponseFrame(const LoginResponse& response,
                              std::vector<uint8_t>& frame,
                              std::string& error) {
    if (!validateLoginResponse(response, error)) return false;
    frame.assign(FRAME_HEADER_SIZE + response.payload.size(), 0);
    writeFrameU16(frame, 0, response.version);
    writeFrameU16(frame, 2, static_cast<uint16_t>(response.status));
    writeFrameU64(frame, 4, response.sessionId);
    writeFrameU32(frame, 12, static_cast<uint32_t>(response.payload.size()));
    for (size_t i = 0; i < response.payload.size(); ++i) {
        frame[FRAME_HEADER_SIZE + i] = static_cast<uint8_t>(response.payload[i]);
    }
    error.clear();
    return true;
}

bool decodeLoginResponseFrame(const std::vector<uint8_t>& frame,
                              LoginResponse& response,
                              std::string& error) {
    if (frame.size() < FRAME_HEADER_SIZE) {
        error = "login response frame header is incomplete";
        return false;
    }
    const uint32_t payloadLength = readFrameU32(frame, 12);
    if (payloadLength > MAX_COMMAND_PAYLOAD ||
        frame.size() != FRAME_HEADER_SIZE + payloadLength) {
        error = "login response frame length is invalid";
        return false;
    }
    response.version = readFrameU16(frame, 0);
    response.status = static_cast<LoginResponseStatus>(readFrameU16(frame, 2));
    response.sessionId = readFrameU64(frame, 4);
    response.payload.assign(frame.begin() + FRAME_HEADER_SIZE, frame.end());
    return validateLoginResponse(response, error);
}

}

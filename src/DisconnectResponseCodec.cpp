#include "DisconnectResponseCodec.h"

#include "FrameHeaderBinary.h"

namespace network {

bool encodeDisconnectResponseFrame(const DisconnectResponse& response,
                                   std::vector<uint8_t>& frame,
                                   std::string& error) {
    if (!validateDisconnectResponse(response, error)) return false;
    frame.assign(FRAME_HEADER_SIZE + response.payload.size(), 0);
    writeFrameU16(frame, 0, response.version);
    writeFrameU16(frame, 2, static_cast<uint16_t>(CommandType::Disconnect));
    writeFrameU64(frame, 4, response.sessionId);
    writeFrameU32(frame, 12, static_cast<uint32_t>(response.payload.size()));
    for (size_t i = 0; i < response.payload.size(); ++i) {
        frame[FRAME_HEADER_SIZE + i] = static_cast<uint8_t>(response.payload[i]);
    }
    error.clear();
    return true;
}

bool decodeDisconnectResponseFrame(const std::vector<uint8_t>& frame,
                                   DisconnectResponse& response,
                                   std::string& error) {
    if (frame.size() < FRAME_HEADER_SIZE) {
        error = "disconnect response frame header is incomplete";
        return false;
    }
    const uint32_t payloadLength = readFrameU32(frame, 12);
    if (payloadLength > MAX_COMMAND_PAYLOAD ||
        frame.size() != FRAME_HEADER_SIZE + payloadLength) {
        error = "disconnect response frame length is invalid";
        return false;
    }
    if (readFrameU16(frame, 2) != static_cast<uint16_t>(CommandType::Disconnect)) {
        error = "frame is not a disconnect response";
        return false;
    }
    response.version = readFrameU16(frame, 0);
    response.sessionId = readFrameU64(frame, 4);
    response.payload.assign(frame.begin() + FRAME_HEADER_SIZE, frame.end());
    response.status = response.payload.empty()
                          ? DisconnectResponseStatus::Accepted
                          : DisconnectResponseStatus::Rejected;
    return validateDisconnectResponse(response, error);
}

}

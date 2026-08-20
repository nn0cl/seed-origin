#include "NetworkFrameCodec.h"

#include "FrameHeaderBinary.h"

namespace network {

bool encodeFrame(const NetworkCommand& command, std::vector<uint8_t>& frame,
                 std::string& error) {
    if (!validateCommand(command, error)) return false;
    frame.assign(FRAME_HEADER_SIZE + command.payload.size(), 0);
    writeFrameU16(frame, 0, command.version);
    writeFrameU16(frame, 2, static_cast<uint16_t>(command.type));
    writeFrameU64(frame, 4, command.sessionId);
    writeFrameU32(frame, 12, static_cast<uint32_t>(command.payload.size()));
    for (size_t i = 0; i < command.payload.size(); ++i) {
        frame[FRAME_HEADER_SIZE + i] = static_cast<uint8_t>(command.payload[i]);
    }
    error.clear();
    return true;
}

bool decodeFrame(const std::vector<uint8_t>& frame, NetworkCommand& command,
                 std::string& error) {
    if (frame.size() < FRAME_HEADER_SIZE) {
        error = "frame header is incomplete";
        return false;
    }
    const uint32_t payloadLength = readFrameU32(frame, 12);
    if (payloadLength > MAX_COMMAND_PAYLOAD ||
        frame.size() != FRAME_HEADER_SIZE + payloadLength) {
        error = "frame length is invalid";
        return false;
    }
    command.version = readFrameU16(frame, 0);
    command.type = static_cast<CommandType>(readFrameU16(frame, 2));
    command.sessionId = readFrameU64(frame, 4);
    command.payload.assign(frame.begin() + FRAME_HEADER_SIZE, frame.end());
    return validateCommand(command, error);
}

}

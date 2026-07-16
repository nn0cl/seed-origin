#include "NetworkFrameCodec.h"

namespace network {

namespace {
void write16(std::vector<uint8_t>& output, size_t offset, uint16_t value) {
    output[offset] = static_cast<uint8_t>((value >> 8) & 0xff);
    output[offset + 1] = static_cast<uint8_t>(value & 0xff);
}

void write32(std::vector<uint8_t>& output, size_t offset, uint32_t value) {
    output[offset] = static_cast<uint8_t>((value >> 24) & 0xff);
    output[offset + 1] = static_cast<uint8_t>((value >> 16) & 0xff);
    output[offset + 2] = static_cast<uint8_t>((value >> 8) & 0xff);
    output[offset + 3] = static_cast<uint8_t>(value & 0xff);
}

void write64(std::vector<uint8_t>& output, size_t offset, int64_t value) {
    const uint64_t bits = static_cast<uint64_t>(value);
    for (size_t i = 0; i < 8; ++i) {
        output[offset + i] = static_cast<uint8_t>((bits >> (56 - i * 8)) & 0xff);
    }
}

uint16_t read16(const std::vector<uint8_t>& input, size_t offset) {
    return static_cast<uint16_t>((input[offset] << 8) | input[offset + 1]);
}

uint32_t read32(const std::vector<uint8_t>& input, size_t offset) {
    return (static_cast<uint32_t>(input[offset]) << 24) |
           (static_cast<uint32_t>(input[offset + 1]) << 16) |
           (static_cast<uint32_t>(input[offset + 2]) << 8) |
           static_cast<uint32_t>(input[offset + 3]);
}

int64_t read64(const std::vector<uint8_t>& input, size_t offset) {
    uint64_t bits = 0;
    for (size_t i = 0; i < 8; ++i) bits = (bits << 8) | input[offset + i];
    return static_cast<int64_t>(bits);
}
}

bool encodeFrame(const NetworkCommand& command, std::vector<uint8_t>& frame,
                 std::string& error) {
    if (!validateCommand(command, error)) return false;
    frame.assign(FRAME_HEADER_SIZE + command.payload.size(), 0);
    write16(frame, 0, command.version);
    write16(frame, 2, static_cast<uint16_t>(command.type));
    write64(frame, 4, command.sessionId);
    write32(frame, 12, static_cast<uint32_t>(command.payload.size()));
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
    const uint32_t payloadLength = read32(frame, 12);
    if (payloadLength > MAX_COMMAND_PAYLOAD ||
        frame.size() != FRAME_HEADER_SIZE + payloadLength) {
        error = "frame length is invalid";
        return false;
    }
    command.version = read16(frame, 0);
    command.type = static_cast<CommandType>(read16(frame, 2));
    command.sessionId = read64(frame, 4);
    command.payload.assign(frame.begin() + FRAME_HEADER_SIZE, frame.end());
    return validateCommand(command, error);
}

}

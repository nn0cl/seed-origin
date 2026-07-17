#include "WorldUpdateFrameCodec.h"

namespace network {
namespace {

void write16(std::vector<uint8_t>& output, size_t offset, uint16_t value) {
    output[offset] = static_cast<uint8_t>(value >> 8);
    output[offset + 1] = static_cast<uint8_t>(value);
}

void write32(std::vector<uint8_t>& output, size_t offset, uint32_t value) {
    for (size_t i = 0; i < 4; ++i)
        output[offset + i] = static_cast<uint8_t>(value >> (24 - i * 8));
}

void write64(std::vector<uint8_t>& output, size_t offset, uint64_t value) {
    for (size_t i = 0; i < 8; ++i)
        output[offset + i] = static_cast<uint8_t>(value >> (56 - i * 8));
}

uint16_t read16(const std::vector<uint8_t>& input, size_t offset) {
    return static_cast<uint16_t>((input[offset] << 8) | input[offset + 1]);
}

uint32_t read32(const std::vector<uint8_t>& input, size_t offset) {
    uint32_t value = 0;
    for (size_t i = 0; i < 4; ++i) value = (value << 8) | input[offset + i];
    return value;
}

uint64_t read64(const std::vector<uint8_t>& input, size_t offset) {
    uint64_t value = 0;
    for (size_t i = 0; i < 8; ++i) value = (value << 8) | input[offset + i];
    return value;
}

}

bool encodeWorldUpdateFrame(const WorldUpdate& update,
                            std::vector<uint8_t>& frame, std::string& error) {
    if (!validateWorldUpdate(update, error)) return false;
    if (update.payload.size() > UINT32_MAX) {
        error = "world update payload is too large for a frame";
        return false;
    }
    frame.assign(WORLD_UPDATE_FRAME_HEADER_SIZE + update.payload.size(), 0);
    write16(frame, 0, WORLD_UPDATE_FRAME_MAGIC);
    write16(frame, 2, update.version);
    write16(frame, 4, static_cast<uint16_t>(update.kind));
    write16(frame, 6, 0);
    write64(frame, 8, update.worldTick);
    write64(frame, 16, update.sequence);
    write64(frame, 24, update.eventId);
    write32(frame, 32, static_cast<uint32_t>(update.payload.size()));
    for (size_t i = 0; i < update.payload.size(); ++i)
        frame[WORLD_UPDATE_FRAME_HEADER_SIZE + i] =
            static_cast<uint8_t>(update.payload[i]);
    error.clear();
    return true;
}

bool decodeWorldUpdateFrame(const std::vector<uint8_t>& frame,
                            WorldUpdate& update, std::string& error) {
    if (frame.size() < WORLD_UPDATE_FRAME_HEADER_SIZE) {
        error = "world update frame header is incomplete";
        return false;
    }
    if (read16(frame, 0) != WORLD_UPDATE_FRAME_MAGIC || read16(frame, 6) != 0) {
        error = "world update frame identity is invalid";
        return false;
    }
    const uint32_t payloadLength = read32(frame, 32);
    if (payloadLength > MAX_UPDATE_PAYLOAD ||
        frame.size() != WORLD_UPDATE_FRAME_HEADER_SIZE + payloadLength) {
        error = "world update frame length is invalid";
        return false;
    }
    update.version = read16(frame, 2);
    update.kind = static_cast<UpdateKind>(read16(frame, 4));
    update.worldTick = read64(frame, 8);
    update.sequence = read64(frame, 16);
    update.eventId = read64(frame, 24);
    update.payload.assign(frame.begin() + WORLD_UPDATE_FRAME_HEADER_SIZE,
                          frame.end());
    return validateWorldUpdate(update, error);
}

}

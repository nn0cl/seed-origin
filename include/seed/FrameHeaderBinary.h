#ifndef SEED_FRAME_HEADER_BINARY_H
#define SEED_FRAME_HEADER_BINARY_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace network {

inline void writeFrameU16(std::vector<uint8_t>& output, size_t offset,
                          uint16_t value) {
    output[offset] = static_cast<uint8_t>((value >> 8) & 0xff);
    output[offset + 1] = static_cast<uint8_t>(value & 0xff);
}

inline void writeFrameU32(std::vector<uint8_t>& output, size_t offset,
                          uint32_t value) {
    output[offset] = static_cast<uint8_t>((value >> 24) & 0xff);
    output[offset + 1] = static_cast<uint8_t>((value >> 16) & 0xff);
    output[offset + 2] = static_cast<uint8_t>((value >> 8) & 0xff);
    output[offset + 3] = static_cast<uint8_t>(value & 0xff);
}

inline void writeFrameU64(std::vector<uint8_t>& output, size_t offset,
                          int64_t value) {
    const uint64_t bits = static_cast<uint64_t>(value);
    for (size_t i = 0; i < 8; ++i) {
        output[offset + i] =
            static_cast<uint8_t>((bits >> (56 - i * 8)) & 0xff);
    }
}

inline uint16_t readFrameU16(const std::vector<uint8_t>& input, size_t offset) {
    return static_cast<uint16_t>((input[offset] << 8) | input[offset + 1]);
}

inline uint32_t readFrameU32(const std::vector<uint8_t>& input, size_t offset) {
    return (static_cast<uint32_t>(input[offset]) << 24) |
           (static_cast<uint32_t>(input[offset + 1]) << 16) |
           (static_cast<uint32_t>(input[offset + 2]) << 8) |
           static_cast<uint32_t>(input[offset + 3]);
}

inline int64_t readFrameU64(const std::vector<uint8_t>& input, size_t offset) {
    uint64_t bits = 0;
    for (size_t i = 0; i < 8; ++i) bits = (bits << 8) | input[offset + i];
    return static_cast<int64_t>(bits);
}

}

#endif

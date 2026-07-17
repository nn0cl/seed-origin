#ifndef SEED_NETWORK_FRAME_CODEC_H
#define SEED_NETWORK_FRAME_CODEC_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "NetworkCommand.h"

namespace network {

inline constexpr std::size_t FRAME_HEADER_SIZE = 16;

[[nodiscard]] bool encodeFrame(const NetworkCommand& command, std::vector<uint8_t>& frame,
                               std::string& error);
[[nodiscard]] bool decodeFrame(const std::vector<uint8_t>& frame, NetworkCommand& command,
                               std::string& error);

}

#endif

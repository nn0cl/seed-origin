#ifndef SEED_WORLD_UPDATE_FRAME_CODEC_H
#define SEED_WORLD_UPDATE_FRAME_CODEC_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "WorldUpdate.h"

namespace network {

inline constexpr std::uint16_t WORLD_UPDATE_FRAME_MAGIC = 0x5755;
inline constexpr std::size_t WORLD_UPDATE_FRAME_HEADER_SIZE = 36;

[[nodiscard]] bool encodeWorldUpdateFrame(const WorldUpdate& update,
                                          std::vector<uint8_t>& frame, std::string& error);
[[nodiscard]] bool decodeWorldUpdateFrame(const std::vector<uint8_t>& frame,
                                          WorldUpdate& update, std::string& error);

}

#endif

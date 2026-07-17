#ifndef SEED_PROTOCOL_H
#define SEED_PROTOCOL_H

#include <cstddef>
#include <cstdint>

namespace network {

// Single source of truth for wire-protocol constants shared by network
// commands and world updates. Both NetworkCommand.h and WorldUpdate.h include
// this header so any consumer of either envelope sees the same version.
inline constexpr std::uint16_t CURRENT_PROTOCOL_VERSION = 1;

}

#endif

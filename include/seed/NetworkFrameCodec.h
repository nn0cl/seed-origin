#ifndef SEED_NETWORK_FRAME_CODEC_H
#define SEED_NETWORK_FRAME_CODEC_H

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "NetworkCommand.h"

namespace network {

static const size_t FRAME_HEADER_SIZE = 16;

bool encodeFrame(const NetworkCommand& command, std::vector<uint8_t>& frame,
                 std::string& error);
bool decodeFrame(const std::vector<uint8_t>& frame, NetworkCommand& command,
                 std::string& error);

}

#endif

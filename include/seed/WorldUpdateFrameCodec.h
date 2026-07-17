#ifndef SEED_WORLD_UPDATE_FRAME_CODEC_H
#define SEED_WORLD_UPDATE_FRAME_CODEC_H

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

#include "WorldUpdate.h"

namespace network {

static const uint16_t WORLD_UPDATE_FRAME_MAGIC = 0x5755;
static const size_t WORLD_UPDATE_FRAME_HEADER_SIZE = 36;

bool encodeWorldUpdateFrame(const WorldUpdate& update,
                            std::vector<uint8_t>& frame, std::string& error);
bool decodeWorldUpdateFrame(const std::vector<uint8_t>& frame,
                            WorldUpdate& update, std::string& error);

}

#endif

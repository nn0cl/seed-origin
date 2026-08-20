#ifndef SEED_DISCONNECT_RESPONSE_CODEC_H
#define SEED_DISCONNECT_RESPONSE_CODEC_H

#include <string>
#include <vector>

#include "DisconnectResponse.h"
#include "NetworkFrameCodec.h"

namespace network {

[[nodiscard]] bool encodeDisconnectResponseFrame(
    const DisconnectResponse& response, std::vector<uint8_t>& frame,
    std::string& error);
[[nodiscard]] bool decodeDisconnectResponseFrame(
    const std::vector<uint8_t>& frame, DisconnectResponse& response,
    std::string& error);

}

#endif

#ifndef SEED_LOGIN_RESPONSE_CODEC_H
#define SEED_LOGIN_RESPONSE_CODEC_H

#include <string>
#include <vector>

#include "LoginResponse.h"
#include "NetworkFrameCodec.h"

namespace network {

[[nodiscard]] bool encodeLoginResponseFrame(const LoginResponse& response,
                                             std::vector<uint8_t>& frame,
                                             std::string& error);
[[nodiscard]] bool decodeLoginResponseFrame(const std::vector<uint8_t>& frame,
                                             LoginResponse& response,
                                             std::string& error);

}

#endif

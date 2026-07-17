#ifndef SEED_LOGIN_RESPONSE_H
#define SEED_LOGIN_RESPONSE_H

#include <stdint.h>
#include <string>

#include "NetworkCommand.h"

namespace network {

enum class LoginResponseStatus {
    Accepted = 1,
    Rejected = 2
};

struct LoginResponse {
    uint16_t version;
    LoginResponseStatus status;
    int64_t sessionId;
    std::string payload;
};

bool validateLoginResponse(const LoginResponse& response, std::string& error);

}

#endif

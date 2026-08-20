#ifndef SEED_DISCONNECT_RESPONSE_H
#define SEED_DISCONNECT_RESPONSE_H

#include <cstdint>
#include <string>

#include "NetworkCommand.h"

namespace network {

enum class DisconnectResponseStatus {
    Accepted = 1,
    Rejected = 2
};

struct DisconnectResponse {
    uint16_t version;
    DisconnectResponseStatus status;
    int64_t sessionId;
    std::string payload;
};

[[nodiscard]] bool validateDisconnectResponse(const DisconnectResponse& response,
                                              std::string& error);

}

#endif

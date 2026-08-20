#include "DisconnectResponse.h"

namespace network {

bool validateDisconnectResponse(const DisconnectResponse& response,
                                std::string& error) {
    if (response.version != CURRENT_PROTOCOL_VERSION) {
        error = "unsupported disconnect response version";
        return false;
    }
    if (response.payload.size() > MAX_COMMAND_PAYLOAD) {
        error = "disconnect response payload is too large";
        return false;
    }
    if (response.status == DisconnectResponseStatus::Accepted) {
        if (response.sessionId <= 0 || !response.payload.empty()) {
            error = "accepted disconnect response is invalid";
            return false;
        }
        error.clear();
        return true;
    }
    if (response.status == DisconnectResponseStatus::Rejected) {
        if (response.payload.empty()) {
            error = "rejected disconnect response is invalid";
            return false;
        }
        error.clear();
        return true;
    }
    error = "unknown disconnect response status";
    return false;
}

}

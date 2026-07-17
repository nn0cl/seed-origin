#include "LoginResponse.h"

namespace network {

bool validateLoginResponse(const LoginResponse& response, std::string& error) {
    if (response.version != CURRENT_PROTOCOL_VERSION) {
        error = "unsupported login response version";
        return false;
    }
    if (response.payload.size() > MAX_COMMAND_PAYLOAD) {
        error = "login response payload is too large";
        return false;
    }
    if (response.status == LoginResponseStatus::Accepted) {
        if (response.sessionId <= 0 || !response.payload.empty()) {
            error = "accepted login response is invalid";
            return false;
        }
        error.clear();
        return true;
    }
    if (response.status == LoginResponseStatus::Rejected) {
        if (response.sessionId != 0 || response.payload.empty()) {
            error = "rejected login response is invalid";
            return false;
        }
        error.clear();
        return true;
    }
    error = "unknown login response status";
    return false;
}

}

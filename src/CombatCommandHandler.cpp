#include "CombatCommandHandler.h"

#include <cmath>
#include <sstream>

namespace server {

namespace {

bool parseAttack(const std::string& payload, std::string& requestId,
                 int64_t& targetId, float& power) {
    const std::string::size_type requestSeparator = payload.find('|');
    if (requestSeparator == std::string::npos) return false;
    requestId = payload.substr(0, requestSeparator);
    if (requestId.empty() || requestId.size() > 64) return false;
    char separator = 0;
    std::istringstream input(payload.substr(requestSeparator + 1));
    if (!(input >> targetId >> separator) || separator != ',' || !(input >> power)) {
        return false;
    }
    input >> std::ws;
    return input.eof() && targetId > 0 && std::isfinite(power) &&
           power > 0.0f && power <= MAX_COMBAT_POWER;
}

bool parseSpell(const std::string& payload, std::string& requestId,
                int64_t& targetId,
                std::string& element, float& power) {
    const std::string::size_type requestSeparator = payload.find('|');
    if (requestSeparator == std::string::npos) return false;
    requestId = payload.substr(0, requestSeparator);
    if (requestId.empty() || requestId.size() > 64) return false;
    char separator = 0;
    std::istringstream input(payload.substr(requestSeparator + 1));
    if (!(input >> targetId >> separator) || separator != ',' ||
        !(input >> element >> separator) || separator != ',' || !(input >> power)) {
        return false;
    }
    input >> std::ws;
    return input.eof() && targetId > 0 && !element.empty() &&
           std::isfinite(power) && power > 0.0f && power <= MAX_COMBAT_POWER;
}

}

CombatCommandHandler::CombatCommandHandler(WorldInputQueue& inputQueue)
    : inputQueue(inputQueue) {}

CombatCommandResult CombatCommandHandler::handle(const network::NetworkCommand& command) {
    CombatCommandResult result = {false, std::string()};
    if (command.type != network::CommandType::Attack &&
        command.type != network::CommandType::CastSpell) {
        result.error = "command is not attack or spell";
        return result;
    }
    if (!network::validateCommand(command, result.error)) return result;

    int64_t targetId = 0;
    float power = 0.0f;
    std::string requestId;
    if (command.type == network::CommandType::Attack) {
        if (!parseAttack(command.payload, requestId, targetId, power) ||
            !inputQueue.enqueueCombat(command.sessionId, targetId, power,
                                       requestId)) {
            result.error = "attack payload or input is invalid";
            return result;
        }
    } else {
        std::string element;
        if (!parseSpell(command.payload, requestId, targetId, element, power) ||
            !inputQueue.enqueueSpell(command.sessionId, targetId, element, power,
                                     requestId)) {
            result.error = "spell payload or input is invalid";
            return result;
        }
    }
    result.accepted = true;
    return result;
}

}

#include "MovementCommandHandler.h"

#include <cmath>
#include <sstream>

namespace server {

namespace {

bool parseDelta(const std::string& payload, float& dx, float& dy, float& dz) {
    std::istringstream input(payload);
    char separator = 0;
    if (!(input >> dx >> separator) || separator != ',') return false;
    if (!(input >> dy >> separator) || separator != ',') return false;
    if (!(input >> dz)) return false;
    input >> std::ws;
    if (!input.eof()) return false;
    return std::isfinite(dx) && std::isfinite(dy) && std::isfinite(dz);
}

bool withinLimit(float value) {
    return std::fabs(value) <= MAX_MOVE_DELTA;
}

}

MovementCommandHandler::MovementCommandHandler(Field& field)
    : field(field), intentQueue(nullptr), worldInputQueue(nullptr) {}

MovementCommandHandler::MovementCommandHandler(MovementIntentQueue& intentQueue)
    : field(*Field::getInstance()), intentQueue(&intentQueue), worldInputQueue(nullptr) {}

MovementCommandHandler::MovementCommandHandler(WorldInputQueue& worldInputQueue)
    : field(*Field::getInstance()), intentQueue(nullptr),
      worldInputQueue(&worldInputQueue) {}

MovementResult MovementCommandHandler::handle(const network::NetworkCommand& command) {
    MovementResult result = {false, std::string()};
    if (command.type != network::CommandType::Move) {
        result.error = "command is not a move";
        return result;
    }
    if (!network::validateCommand(command, result.error)) return result;

    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
    if (!parseDelta(command.payload, dx, dy, dz)) {
        result.error = "move payload must be finite dx,dy,dz";
        return result;
    }
    if (!withinLimit(dx) || !withinLimit(dy) || !withinLimit(dz) ||
        !isValidMovementDelta(dx, dy, dz)) {
        result.error = "move delta exceeds limit";
        return result;
    }
    if (worldInputQueue != nullptr) {
        if (!worldInputQueue->enqueueMovement(command.sessionId, dx, dy, dz)) {
            result.error = "movement world input queue is full";
            return result;
        }
    } else if (intentQueue != nullptr) {
        if (!intentQueue->enqueue(command.sessionId, dx, dy, dz)) {
            result.error = "movement intent queue is full";
            return result;
        }
    } else if (!field.queueMovement(command.sessionId, dx, dy, dz)) {
        result.error = "session player is not present in the field";
        return result;
    }
    result.accepted = true;
    return result;
}

}

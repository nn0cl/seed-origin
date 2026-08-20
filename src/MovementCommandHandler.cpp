#include "MovementCommandHandler.h"

#include "ClientInputSequence.h"

#include <cmath>

namespace server {

namespace {

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
    MovementResult result = {false, false, std::string()};
    if (command.type != network::CommandType::Move) {
        result.error = "command is not a move";
        return result;
    }
    if (!network::validateCommand(command, result.error)) return result;

    uint64_t clientInputSequence = 0;
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
    if (!parseMovePayload(command.payload, clientInputSequence, dx, dy, dz)) {
        result.error = "move payload must be finite dx,dy,dz";
        return result;
    }
    if (!withinLimit(dx) || !withinLimit(dy) || !withinLimit(dz) ||
        !isValidMovementDelta(dx, dy, dz)) {
        result.error = "move delta exceeds limit";
        return result;
    }
    if (worldInputQueue != nullptr) {
        const MovementEnqueueResult status =
            worldInputQueue->enqueueSequencedMovement(
                command.sessionId, dx, dy, dz, clientInputSequence);
        if (status == MovementEnqueueResult::IgnoredStale) {
            result.ignored = true;
            result.error = "duplicate or stale client input sequence";
            return result;
        }
        if (status != MovementEnqueueResult::Enqueued) {
            result.error = clientInputSequence != 0
                ? "client input sequence was rejected"
                : "movement world input queue is full";
            return result;
        }
    } else if (intentQueue != nullptr) {
        if (!intentQueue->enqueue(command.sessionId, dx, dy, dz,
                                  clientInputSequence)) {
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

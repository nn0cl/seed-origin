#ifndef SEED_MOVEMENT_COMMAND_HANDLER_H
#define SEED_MOVEMENT_COMMAND_HANDLER_H

#include <string>

#include "Field.h"
#include "NetworkCommand.h"
#include "MovementIntentQueue.h"

namespace server {

inline constexpr float MAX_MOVE_DELTA = MAX_MOVE_DISTANCE_PER_FRAME;

struct MovementResult {
    bool accepted;
    std::string error;
};

class MovementCommandHandler {
public:
    explicit MovementCommandHandler(Field& field);
    explicit MovementCommandHandler(MovementIntentQueue& intentQueue);
    MovementResult handle(const network::NetworkCommand& command);

private:
    Field& field;
    MovementIntentQueue* intentQueue;
};

}

#endif

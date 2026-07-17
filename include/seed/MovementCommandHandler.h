#ifndef SEED_MOVEMENT_COMMAND_HANDLER_H
#define SEED_MOVEMENT_COMMAND_HANDLER_H

#include <string>

#include "Field.h"
#include "NetworkCommand.h"
#include "MovementIntentQueue.h"

namespace server {

static const float MAX_MOVE_DELTA = 100.0f;

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

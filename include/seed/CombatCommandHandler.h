#ifndef SEED_COMBAT_COMMAND_HANDLER_H
#define SEED_COMBAT_COMMAND_HANDLER_H

#include <string>

#include "NetworkCommand.h"
#include "WorldInputQueue.h"

namespace server {

static const float MAX_COMBAT_POWER = 100000.0f;

struct CombatCommandResult {
    bool accepted;
    std::string error;
};

class CombatCommandHandler {
public:
    explicit CombatCommandHandler(WorldInputQueue& inputQueue);
    CombatCommandResult handle(const network::NetworkCommand& command);

private:
    WorldInputQueue& inputQueue;
};

}

#endif

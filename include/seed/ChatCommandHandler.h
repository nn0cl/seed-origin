#ifndef SEED_CHAT_COMMAND_HANDLER_H
#define SEED_CHAT_COMMAND_HANDLER_H

#include <string>

#include "NetworkCommand.h"
#include "WorldInputQueue.h"

namespace server {

class ChatCommandHandler {
public:
    explicit ChatCommandHandler(WorldInputQueue& queue);
    bool handle(const network::NetworkCommand& command, std::string& error);

private:
    WorldInputQueue& inputQueue;
};

}

#endif

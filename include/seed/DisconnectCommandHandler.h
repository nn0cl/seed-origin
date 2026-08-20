#ifndef SEED_DISCONNECT_COMMAND_HANDLER_H
#define SEED_DISCONNECT_COMMAND_HANDLER_H

#include "NetworkCommand.h"
#include "SessionRegistry.h"

namespace server {

struct DisconnectResult {
    bool accepted;
    std::string error;
    session::SessionInfo session;
};

class DisconnectCommandHandler {
public:
    explicit DisconnectCommandHandler(session::SessionRegistry& registry);
    DisconnectResult handle(const network::NetworkCommand& command);

private:
    session::SessionRegistry& registry;
};

}

#endif

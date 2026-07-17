#ifndef SEED_SERVER_RUNTIME_H
#define SEED_SERVER_RUNTIME_H

#include <stddef.h>
#include <stdint.h>
#include <deque>
#include <vector>

#include "Connection.h"
#include "ClientSession.h"
#include "NetworkCommand.h"
#include "ServerCommandDispatcher.h"

namespace server {

static const size_t MAX_PENDING_COMMANDS = 1024;

class ServerRuntime {
public:
    ServerRuntime();
    ~ServerRuntime();

    bool start(uint16_t port);
    bool stop();
    bool isRunning() const;
    bool submit(const network::NetworkCommand& command);
    ReceiveStatus ingest(ClientSession& session, std::string& error);
    std::vector<network::NetworkCommand> drainCommands();
    std::vector<CommandDispatchResult> dispatchPendingCommands(
        ServerCommandDispatcher& dispatcher);
    size_t pendingCommandCount() const;

private:
    Connection listener;
    bool running;
    std::deque<network::NetworkCommand> pendingCommands;
};

}

#endif

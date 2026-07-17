#ifndef SEED_SERVER_RUNTIME_H
#define SEED_SERVER_RUNTIME_H

#include <stddef.h>
#include <stdint.h>
#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Connection.h"
#include "ClientSession.h"
#include "NetworkCommand.h"
#include "ServerCommandDispatcher.h"
#include "SessionLifecycle.h"

namespace server {

static const size_t MAX_PENDING_COMMANDS = 1024;
static const size_t MAX_ACCEPTS_PER_FRAME = 64;

class ServerRuntime {
public:
    ServerRuntime();
    ~ServerRuntime();

    bool start(uint16_t port);
    bool stop();
    bool isRunning() const;
    AcceptStatus acceptPendingClient(uint64_t& connectionId, std::string& error);
    ClientSession* clientSession(uint64_t connectionId);
    size_t connectedClientCount() const;
    size_t removeClosedClients(session::SessionRegistry& registry);
    bool stop(session::SessionRegistry& registry);
    size_t processClientFrames(ServerCommandDispatcher& dispatcher, std::string& error);
    size_t processFrame(ServerCommandDispatcher& dispatcher, std::string& error);
    bool submit(const network::NetworkCommand& command);
    ReceiveStatus ingest(ClientSession& session, std::string& error);
    std::vector<network::NetworkCommand> drainCommands();
    std::vector<CommandDispatchResult> dispatchPendingCommands(
        ServerCommandDispatcher& dispatcher);
    size_t pendingCommandCount() const;

private:
    struct PendingCommand {
        uint64_t connectionId;
        network::NetworkCommand command;
    };

    bool enqueueCommands(uint64_t connectionId,
                         const std::vector<network::NetworkCommand>& commands,
                         std::string& error);

    Connection listener;
    bool running;
    uint64_t nextConnectionId;
    std::map<uint64_t, std::unique_ptr<ClientSession> > clients;
    SessionLifecycle lifecycle;
    std::deque<PendingCommand> pendingCommands;
};

}

#endif

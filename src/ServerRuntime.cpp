#include "ServerRuntime.h"
#include "ServerCommandDispatcher.h"

namespace server {

ServerRuntime::ServerRuntime() : running(false) {}

ServerRuntime::~ServerRuntime() {
    stop();
}

bool ServerRuntime::start(uint16_t port) {
    if (running || !listener.open(port)) return false;
    running = true;
    return true;
}

bool ServerRuntime::stop() {
    pendingCommands.clear();
    const bool closed = listener.closeSocket();
    running = false;
    return closed;
}

bool ServerRuntime::isRunning() const {
    return running;
}

bool ServerRuntime::submit(const network::NetworkCommand& command) {
    if (!running || pendingCommands.size() >= MAX_PENDING_COMMANDS) return false;
    std::string error;
    if (!network::validateCommand(command, error)) return false;
    pendingCommands.push_back(command);
    return true;
}

std::vector<network::NetworkCommand> ServerRuntime::drainCommands() {
    std::vector<network::NetworkCommand> drained;
    drained.reserve(pendingCommands.size());
    while (!pendingCommands.empty()) {
        drained.push_back(pendingCommands.front());
        pendingCommands.pop_front();
    }
    return drained;
}

std::vector<CommandDispatchResult> ServerRuntime::dispatchPendingCommands(
    ServerCommandDispatcher& dispatcher) {
    return dispatcher.dispatchAll(drainCommands());
}

size_t ServerRuntime::pendingCommandCount() const {
    return pendingCommands.size();
}

}

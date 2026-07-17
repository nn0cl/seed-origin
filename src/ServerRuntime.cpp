#include "ServerRuntime.h"
#include "LoginResponseCodec.h"
#include "ServerCommandDispatcher.h"
#include "WorldUpdateFrameCodec.h"

#include <limits>
#include <utility>

namespace server {

ServerRuntime::ServerRuntime()
    : running(false), nextConnectionId(1), clients(), lifecycle(), inputQueue(),
      inputTick(inputQueue) {}

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
    inputQueue.clear();
    clients.clear();
    lifecycle.clear();
    const bool closed = listener.closeSocket();
    running = false;
    return closed;
}

bool ServerRuntime::stop(session::SessionRegistry& registry) {
    pendingCommands.clear();
    inputQueue.clear();
    clients.clear();
    lifecycle.clear(registry);
    const bool closed = listener.closeSocket();
    running = false;
    return closed;
}

bool ServerRuntime::isRunning() const {
    return running;
}

AcceptStatus ServerRuntime::acceptPendingClient(uint64_t& connectionId,
                                                std::string& error) {
    connectionId = 0;
    if (!running) {
        error = "server runtime is stopped";
        return AcceptStatus::Failed;
    }
    int clientSocket = -1;
    const AcceptStatus status = listener.acceptClient(clientSocket);
    if (status != AcceptStatus::Accepted) return status;
    if (nextConnectionId == std::numeric_limits<uint64_t>::max()) {
        ::close(clientSocket);
        error = "connection id allocation failed";
        return AcceptStatus::Failed;
    }
    std::unique_ptr<ClientSession> session(new ClientSession(clientSocket));
    if (!session->isOpen()) {
        error = "accepted client session could not be initialized";
        return AcceptStatus::Failed;
    }
    connectionId = nextConnectionId++;
    clients[connectionId] = std::move(session);
    error.clear();
    return AcceptStatus::Accepted;
}

ClientSession* ServerRuntime::clientSession(uint64_t connectionId) {
    const std::map<uint64_t, std::unique_ptr<ClientSession> >::iterator found =
        clients.find(connectionId);
    return found == clients.end() ? 0 : found->second.get();
}

size_t ServerRuntime::connectedClientCount() const {
    return clients.size();
}

size_t ServerRuntime::removeClosedClients(session::SessionRegistry& registry) {
    size_t removed = 0;
    for (std::map<uint64_t, std::unique_ptr<ClientSession> >::iterator it = clients.begin();
         it != clients.end();) {
        if (it->second->isOpen()) {
            ++it;
            continue;
        }
        lifecycle.disconnect(it->first, registry);
        it = clients.erase(it);
        ++removed;
    }
    return removed;
}

bool ServerRuntime::submit(const network::NetworkCommand& command) {
    if (!running || pendingCommands.size() >= MAX_PENDING_COMMANDS) return false;
    std::string error;
    if (!network::validateCommand(command, error)) return false;
    pendingCommands.push_back({0, command});
    return true;
}

bool ServerRuntime::submitAction(const Action& action) {
    return running && inputTick.submitAction(action);
}

bool ServerRuntime::submitMovement(int64_t sessionId, float dx, float dy, float dz) {
    return running && inputTick.submitMovement(sessionId, dx, dy, dz);
}

ReceiveStatus ServerRuntime::ingest(ClientSession& session, std::string& error) {
    if (!running) {
        error = "server runtime is stopped";
        return ReceiveStatus::Failed;
    }

    std::vector<network::NetworkCommand> commands;
    const ReceiveStatus status = session.receive(commands, error);
    if (status != ReceiveStatus::Commands) return status;
    if (!enqueueCommands(0, commands, error)) return ReceiveStatus::Failed;
    return ReceiveStatus::Commands;
}

bool ServerRuntime::enqueueCommands(
    uint64_t connectionId,
    const std::vector<network::NetworkCommand>& commands,
    std::string& error) {
    if (commands.size() > MAX_PENDING_COMMANDS - pendingCommands.size()) {
        error = "server command queue is full";
        return false;
    }

    for (std::vector<network::NetworkCommand>::const_iterator it = commands.begin();
         it != commands.end(); ++it) {
        std::string validationError;
        if (!network::validateCommand(*it, validationError)) {
            error = validationError;
            return false;
        }
    }
    for (std::vector<network::NetworkCommand>::const_iterator it = commands.begin();
         it != commands.end(); ++it) {
        pendingCommands.push_back({connectionId, *it});
    }
    error.clear();
    return true;
}

size_t ServerRuntime::processClientFrames(ServerCommandDispatcher& dispatcher,
                                          std::string& error) {
    if (!running) {
        error = "server runtime is stopped";
        return 0;
    }

    dispatcher.bindWorldInputQueue(inputQueue);
    for (std::map<uint64_t, std::unique_ptr<ClientSession> >::iterator it = clients.begin();
         it != clients.end(); ++it) {
        std::vector<network::NetworkCommand> commands;
        std::string receiveError;
        const ReceiveStatus status = it->second->receive(commands, receiveError);
        if (status == ReceiveStatus::Failed) {
            if (error.empty()) error = receiveError;
            continue;
        }
        if (status == ReceiveStatus::Commands) {
            std::string enqueueError;
            if (!enqueueCommands(it->first, commands, enqueueError)) {
                if (error.empty()) error = enqueueError;
                continue;
            }
        }
    }

    size_t processed = 0;
    while (!pendingCommands.empty()) {
        const PendingCommand pending = pendingCommands.front();
        pendingCommands.pop_front();
        CommandDispatchResult result = {
            false, pending.command.type, {0, 0, std::string(), false}, std::string()};
        if (pending.command.type == network::CommandType::Login &&
            lifecycle.hasSession(pending.connectionId)) {
            result.error = "connection already has a session";
        } else {
            result = dispatcher.dispatch(pending.command);
        }
        ++processed;
        if (pending.connectionId == 0 || pending.command.type != network::CommandType::Login) {
            continue;
        }
        ClientSession* session = clientSession(pending.connectionId);
        if (session == 0 || !session->isOpen()) continue;
        if (result.accepted) {
            std::string bindingError;
            if (!lifecycle.bind(pending.connectionId, result.session, bindingError)) {
                dispatcher.sessionRegistry().logout(result.session.internalId);
                result.accepted = false;
                result.session = {0, 0, std::string(), false};
                result.error = bindingError;
            }
        }
        const network::LoginResponse response = {
            network::CURRENT_PROTOCOL_VERSION,
            result.accepted ? network::LoginResponseStatus::Accepted
                            : network::LoginResponseStatus::Rejected,
            result.accepted ? result.session.internalId : 0,
            result.accepted ? std::string() : result.error
        };
        std::vector<uint8_t> frame;
        std::string responseError;
        if (!network::encodeLoginResponseFrame(response, frame, responseError) ||
            !session->enqueueFrame(frame, responseError)) {
            if (error.empty()) error = responseError;
        }
    }
    removeClosedClients(dispatcher.sessionRegistry());
    return processed;
}

ServerFrameResult ServerRuntime::processFrame(ServerCommandDispatcher& dispatcher,
                                              std::string& error) {
    ServerFrameResult stopped = {inputTick.currentWorldTick(), 0,
                                 std::vector<WorldInput>()};
    if (!running) {
        error = "server runtime is stopped";
        return stopped;
    }

    size_t accepted = 0;
    for (; accepted < MAX_ACCEPTS_PER_FRAME; ++accepted) {
        uint64_t connectionId = 0;
        std::string acceptError;
        const AcceptStatus status = acceptPendingClient(connectionId, acceptError);
        if (status == AcceptStatus::NoPendingClient) break;
        if (status == AcceptStatus::Failed) {
            if (error.empty()) error = acceptError;
            break;
        }
    }

    const size_t processed = processClientFrames(dispatcher, error);
    for (std::map<uint64_t, std::unique_ptr<ClientSession> >::iterator it = clients.begin();
         it != clients.end(); ++it) {
        std::string sendError;
        const SendStatus status = it->second->flushOutbound(sendError);
        if (status == SendStatus::Failed && error.empty()) error = sendError;
    }
    removeClosedClients(dispatcher.sessionRegistry());
    const WorldFrameInputs worldFrame = inputTick.advanceFrame();
    ServerFrameResult result = {worldFrame.worldTick, accepted + processed,
                                worldFrame.inputs};
    return result;
}

size_t ServerRuntime::publishWorldUpdates(
    const std::vector<network::WorldUpdate>& updates, std::string& error) {
    if (!running) {
        error = "server runtime is stopped";
        return 0;
    }
    size_t published = 0;
    for (std::vector<network::WorldUpdate>::const_iterator update = updates.begin();
         update != updates.end(); ++update) {
        std::vector<uint8_t> frame;
        if (!network::encodeWorldUpdateFrame(*update, frame, error)) return published;
        for (std::map<uint64_t, std::unique_ptr<ClientSession> >::iterator it =
                 clients.begin(); it != clients.end(); ++it) {
            if (!lifecycle.hasSession(it->first)) continue;
            std::string enqueueError;
            if (!it->second->enqueueFrame(frame, enqueueError)) {
                error = enqueueError;
                return published;
            }
            ++published;
        }
    }
    error.clear();
    return published;
}

std::vector<network::NetworkCommand> ServerRuntime::drainCommands() {
    std::vector<network::NetworkCommand> drained;
    drained.reserve(pendingCommands.size());
    while (!pendingCommands.empty()) {
        drained.push_back(pendingCommands.front().command);
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

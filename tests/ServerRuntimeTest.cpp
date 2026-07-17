#include <cassert>
#include <sys/socket.h>
#include <unistd.h>

#include "ServerRuntime.h"
#include "ServerCommandDispatcher.h"
#include "NetworkFrameCodec.h"
#include "LoginResponseCodec.h"

namespace server_runtime_tests {

void rejects_commands_before_start() {
    server::ServerRuntime runtime;
    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "player-a"
    };
    assert(!runtime.submit(command));
    assert(runtime.pendingCommandCount() == 0);
}

void drains_valid_commands_in_fifo_order() {
    server::ServerRuntime runtime;
    network::NetworkCommand first = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "first"
    };
    network::NetworkCommand second = {
        network::CURRENT_PROTOCOL_VERSION,
        network::CommandType::Login,
        0,
        "second"
    };
    assert(runtime.start(0));
    assert(runtime.submit(first));
    assert(runtime.submit(second));
    const std::vector<network::NetworkCommand> commands = runtime.drainCommands();
    assert(commands.size() == 2);
    assert(commands[0].payload == "first");
    assert(commands[1].payload == "second");
    assert(runtime.stop());
}

void dispatches_pending_commands_in_fifo_order() {
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    server::ServerRuntime runtime;
    const std::vector<network::NetworkCommand> commands = {
        {network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0, "first"},
        {network::CURRENT_PROTOCOL_VERSION, network::CommandType::Move, 0, "1,0,0"}
    };

    assert(runtime.start(0));
    assert(runtime.submit(commands[0]));
    assert(runtime.submit(commands[1]));
    const std::vector<server::CommandDispatchResult> results =
        runtime.dispatchPendingCommands(dispatcher);
    assert(results.size() == 2);
    assert(results[0].accepted);
    assert(!results[1].accepted);
    assert(runtime.pendingCommandCount() == 0);
    assert(runtime.stop());
}

void ingests_decoded_commands_from_client_session() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    server::ClientSession session(sockets[1]);
    server::ServerRuntime runtime;
    assert(runtime.start(0));

    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0, "player"};
    std::vector<uint8_t> frame;
    std::string error;
    assert(network::encodeFrame(command, frame, error));
    assert(::send(sockets[0], frame.data(), frame.size(), 0) ==
           static_cast<ssize_t>(frame.size()));
    assert(runtime.ingest(session, error) == server::ReceiveStatus::Commands);
    assert(runtime.pendingCommandCount() == 1);
    assert(runtime.stop());
    ::close(sockets[0]);
}

void dispatches_ingested_login_to_session_registry() {
    server::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    server::ServerRuntime runtime;
    assert(runtime.start(0));

    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0, "player-a"};
    assert(runtime.submit(command));
    const std::vector<server::CommandDispatchResult> results =
        runtime.dispatchPendingCommands(dispatcher);
    assert(results.size() == 1);
    assert(results[0].accepted);
    assert(results[0].session.internalId > 0);
    assert(runtime.pendingCommandCount() == 0);
    assert(runtime.stop());
}

void rejects_client_accept_when_runtime_is_stopped() {
    server::ServerRuntime runtime;
    uint64_t connectionId = 0;
    std::string error;
    assert(runtime.acceptPendingClient(connectionId, error) == server::AcceptStatus::Failed);
    assert(connectionId == 0);
    assert(runtime.connectedClientCount() == 0);
}

void clears_owned_clients_on_stop() {
    server::ServerRuntime runtime;
    session::SessionRegistry registry;
    assert(runtime.start(0));
    assert(runtime.connectedClientCount() == 0);
    assert(runtime.removeClosedClients(registry) == 0);
    assert(runtime.stop());
    assert(runtime.connectedClientCount() == 0);
}

void rejects_client_frame_processing_when_runtime_is_stopped() {
    server::ServerRuntime runtime;
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    std::string error;
    assert(runtime.processClientFrames(dispatcher, error) == 0);
}

void rejects_frame_processing_when_runtime_is_stopped() {
    server::ServerRuntime runtime;
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    std::string error;
    assert(runtime.processFrame(dispatcher, error) == 0);
}

} // namespace server_runtime_tests

#include <cassert>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "ClientTransportShell.h"
#include "Field.h"
#include "FieldSessionPresence.h"
#include "LoginFieldSpawnSettings.h"
#include "LoginResponseCodec.h"
#include "NetworkFrameCodec.h"
#include "Player.h"
#include "Position.h"
#include "ServerCommandDispatcher.h"
#include "ServerRuntime.h"
#include "SessionRegistry.h"
#include "Status.h"
#include "WorldFrameApplier.h"
#include "WorldUpdateFrameCodec.h"

namespace client_transport_shell_tests {
namespace {

void clearFieldPlayers() {
    Field* field = Field::getInstance();
    const std::vector<PlayerPoseSnapshot> poses = field->publicPlayerPoses();
    for (std::size_t i = 0; i < poses.size(); ++i) {
        const int64_t sessionId = poses[i].sessionId;
        Field::unsetPlayer(Player(sessionId, Status(), Position(sessionId, 0, 0, 0)));
    }
}

bool encodeUpdate(const network::WorldUpdate& update, std::vector<uint8_t>& frame) {
    std::string error;
    return network::encodeWorldUpdateFrame(update, frame, error);
}

void serverTick(server::ServerRuntime& runtime,
                server::ServerCommandDispatcher& dispatcher,
                server::WorldFrameApplier& applier,
                server::ServerFrameResult& frame) {
    std::string error;
    frame = runtime.processFrame(dispatcher, error);
    assert(error.empty());
    std::vector<network::WorldUpdate> updates;
    assert(applier.apply(
        server::WorldFrameInputs{frame.worldTick, frame.inputs}, updates, error));
    std::vector<server::MovementAck> acks = applier.ownerMovementAcks();
    assert(applier.capturePublicSnapshotIfNewSessions(
        frame.newAuthenticatedSessions + frame.snapshotRequests, frame.worldTick,
        updates, acks, error));
    if (updates.empty()) return;
    std::string publishError;
    runtime.publishWorldUpdates(updates, acks, publishError);
}

}

void after_reconnect_and_login_writes_request_snapshot_on_the_socket() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    client::ClientTransportShell transport;
    std::string error;
    assert(transport.attachConnectedSocket(sockets[1], error));
    transport.beginReconnect();
    assert(transport.snapshotRequested());
    assert(transport.authState() == client::TransportAuthState::Anonymous);

    int reconnectSockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, reconnectSockets) == 0);
    assert(transport.attachConnectedSocket(reconnectSockets[1], error));
    assert(transport.pump(error));
    uint8_t peek[8];
    const ssize_t idle = ::recv(reconnectSockets[0], peek, sizeof(peek), MSG_DONTWAIT);
    assert(idle < 0);

    const network::LoginResponse accepted = {
        network::CURRENT_PROTOCOL_VERSION, network::LoginResponseStatus::Accepted,
        21, std::string()};
    std::vector<uint8_t> loginFrame;
    assert(network::encodeLoginResponseFrame(accepted, loginFrame, error));
    assert(::send(reconnectSockets[0], loginFrame.data(), loginFrame.size(), 0) ==
           static_cast<ssize_t>(loginFrame.size()));
    assert(transport.pump(error));
    assert(transport.authState() == client::TransportAuthState::LoggedIn);
    assert(transport.sessionId() == 21);

    std::vector<uint8_t> received(64, 0);
    const ssize_t n = ::recv(reconnectSockets[0], received.data(), received.size(), 0);
    assert(n > 0);
    received.resize(static_cast<size_t>(n));
    network::NetworkCommand command = {};
    assert(network::decodeFrame(received, command, error));
    assert(command.version == 1);
    assert(command.type == network::CommandType::RequestSnapshot);
    assert(command.sessionId == 21);
    assert(command.payload.empty());
    ::close(reconnectSockets[0]);
    ::close(sockets[0]);
}

void unauthenticated_reconnect_does_not_write_request_snapshot() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    client::ClientTransportShell transport;
    std::string error;
    transport.beginReconnect();
    assert(transport.attachConnectedSocket(sockets[1], error));
    assert(transport.pump(error));
    assert(transport.authState() == client::TransportAuthState::Anonymous);
    uint8_t peek[8];
    const ssize_t idle = ::recv(sockets[0], peek, sizeof(peek), MSG_DONTWAIT);
    assert(idle < 0);
    ::close(sockets[0]);
}

void snapshot_from_peer_clears_request_after_skipped_events() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    client::ClientTransportShell transport;
    std::string error;
    transport.beginReconnect();
    assert(transport.attachConnectedSocket(sockets[1], error));

    const network::LoginResponse accepted = {
        network::CURRENT_PROTOCOL_VERSION, network::LoginResponseStatus::Accepted,
        21, std::string()};
    std::vector<uint8_t> loginFrame;
    assert(network::encodeLoginResponseFrame(accepted, loginFrame, error));
    assert(::send(sockets[0], loginFrame.data(), loginFrame.size(), 0) ==
           static_cast<ssize_t>(loginFrame.size()));
    assert(transport.pump(error));
    std::vector<uint8_t> requestBytes(64, 0);
    const ssize_t requestSize =
        ::recv(sockets[0], requestBytes.data(), requestBytes.size(), 0);
    assert(requestSize > 0);

    const network::WorldUpdate movement = {
        1, network::UpdateKind::Event, 4, 1, 1,
        "movement=session:21;dx=1;dy=0;dz=0;clientInputSequence=1;inputSequence=1"};
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 4, 2, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "player.count=1;player.0.session=21;player.0.x=0;player.0.y=0;player.0.z=0;"
        "player.0.name=Hero"};
    std::vector<uint8_t> mixed;
    assert(encodeUpdate(movement, mixed));
    std::vector<uint8_t> snapshotBytes;
    assert(encodeUpdate(snapshot, snapshotBytes));
    mixed.insert(mixed.end(), snapshotBytes.begin(), snapshotBytes.end());
    assert(::send(sockets[0], mixed.data(), mixed.size(), 0) ==
           static_cast<ssize_t>(mixed.size()));
    assert(transport.pump(error));
    assert(!transport.snapshotRequested());
    assert(transport.worldReceiver().expectedSequence() == 3);
    assert(transport.worldReceiver().environment().value().players.size() == 1);
    ::close(sockets[0]);
}

void loopback_reconnect_sends_request_snapshot_and_applies_server_snapshot() {
    clearFieldPlayers();
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    server::ServerRuntime runtime;
    server::WorldFrameApplier applier(*Field::getInstance());
    assert(runtime.start(0));
    server::LoginFieldSpawnSettings spawn;
    spawn.playerName = "Hero";
    server::FieldSessionPresence::useSpawnSettings(spawn);
    const uint16_t port = runtime.listeningPort();
    assert(port != 0);

    client::ClientTransportShell transport;
    std::string error;
    assert(transport.connectTcp("127.0.0.1", port, error));
    assert(transport.enqueueLogin("liss0128-rejoin", error));
    assert(transport.flushOutbound(error) == server::SendStatus::Sent);

    server::ServerFrameResult frame = {};
    bool loggedIn = false;
    for (int i = 0; i < 8 && !loggedIn; ++i) {
        serverTick(runtime, dispatcher, applier, frame);
        assert(transport.pump(error));
        loggedIn = transport.authState() == client::TransportAuthState::LoggedIn;
    }
    assert(loggedIn);

    transport.beginReconnect();
    assert(transport.snapshotRequested());
    serverTick(runtime, dispatcher, applier, frame);

    assert(transport.connectTcp("127.0.0.1", port, error));
    assert(transport.enqueueLogin("liss0128-rejoin", error));
    assert(transport.flushOutbound(error) == server::SendStatus::Sent);
    loggedIn = false;
    size_t snapshotRequestsSeen = 0;
    for (int i = 0; i < 8; ++i) {
        serverTick(runtime, dispatcher, applier, frame);
        snapshotRequestsSeen += frame.snapshotRequests;
        assert(transport.pump(error));
        if (transport.authState() == client::TransportAuthState::LoggedIn) {
            loggedIn = true;
        }
        if (loggedIn && !transport.snapshotRequested() && snapshotRequestsSeen >= 1) {
            break;
        }
    }
    assert(loggedIn);
    assert(snapshotRequestsSeen >= 1);
    assert(!transport.snapshotRequested());
    assert(transport.worldReceiver().environment().value().hasLocalPlayer ||
           !transport.worldReceiver().environment().value().players.empty());
    assert(runtime.stop());
    server::FieldSessionPresence::useSpawnSettings(server::LoginFieldSpawnSettings());
    server::FieldSessionPresence::usePlayerIdPort(0);
    clearFieldPlayers();
}

} // namespace client_transport_shell_tests

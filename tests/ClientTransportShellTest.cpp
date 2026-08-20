#include <cassert>
#include <map>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "ChallengeAuthTestFixtures.h"
#include "TransportLoopbackTestSupport.h"
#include "ClientTransportShell.h"
#include "Field.h"
#include "FieldSessionPresence.h"
#include "LoginFieldSpawnSettings.h"
#include "Player.h"
#include "Status.h"
#include "WorldFrameApplier.h"
#include "DisconnectResponseCodec.h"
#include "LoginResponseCodec.h"
#include "NetworkFrameCodec.h"
#include "RegistryGameplaySessionPort.h"
#include "ServerCommandDispatcher.h"
#include "ServerRuntime.h"
#include "SessionRegistry.h"
#include "WorldUpdateFrameCodec.h"

namespace client_transport_shell_tests {
namespace {

using seed_test::clearFieldPlayers;
using seed_test::FakeChallengeClaimPort;
using seed_test::FakeSessionStore;
using seed_test::FixedKeyIssuer;
using seed_test::FixedPlayerIdPort;
using seed_test::FixedWallClock;
using seed_test::resetFieldSessionPresence;
using seed_test::serverTick;

bool encodeUpdate(const network::WorldUpdate& update, std::vector<uint8_t>& frame) {
    std::string error;
    return network::encodeWorldUpdateFrame(update, frame, error);
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
    const FixedPlayerIdPort playerIdPort(9001);
    server::FieldSessionPresence::usePlayerIdPort(&playerIdPort);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("transport-session");
    server::ChallengeSessionLoginService auth(challenges, sessions, keys, clock);
    session::SessionRegistry registry;
    server::RegistryGameplaySessionPort gameplay(registry);
    server::ServerCommandDispatcher dispatcher(registry, auth, gameplay);
    server::ServerRuntime runtime;
    server::WorldFrameApplier applier(*Field::getInstance());
    assert(runtime.start(0));
    server::LoginFieldSpawnSettings spawn;
    spawn.playerName = "Hero";
    server::FieldSessionPresence::useSpawnSettings(spawn);
    const uint16_t port = runtime.listeningPort();
    assert(port != 0);

    const std::string challengeKey = "liss0128-rejoin";
    challenges.putUnclaimed(challengeKey, 42, now + 120);

    client::ClientTransportShell transport;
    std::string error;
    assert(transport.connectTcp("127.0.0.1", port, error));
    assert(transport.enqueueLogin(challengeKey, error));
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

    challenges.putUnclaimed(challengeKey, 42, now + 120);
    assert(transport.connectTcp("127.0.0.1", port, error));
    assert(transport.enqueueLogin(challengeKey, error));
    assert(transport.flushOutbound(error) == server::SendStatus::Sent);
    loggedIn = false;
    size_t snapshotRequestsSeen = 0;
    for (int i = 0; i < 32; ++i) {
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
    resetFieldSessionPresence();
    clearFieldPlayers();
}

void rejected_disconnect_keeps_tcp_accepted_ack_closes_it() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    client::ClientTransportShell transport;
    std::string error;
    assert(transport.attachConnectedSocket(sockets[1], error));

    const network::LoginResponse accepted = {
        network::CURRENT_PROTOCOL_VERSION, network::LoginResponseStatus::Accepted,
        21, std::string()};
    std::vector<uint8_t> loginFrame;
    assert(network::encodeLoginResponseFrame(accepted, loginFrame, error));
    assert(::send(sockets[0], loginFrame.data(), loginFrame.size(), 0) ==
           static_cast<ssize_t>(loginFrame.size()));
    assert(transport.pump(error));
    assert(transport.authState() == client::TransportAuthState::LoggedIn);
    assert(transport.sessionId() == 21);

    assert(transport.enqueueDisconnect(error));
    assert(transport.pump(error));
    assert(transport.authState() == client::TransportAuthState::LoggedIn);
    assert(transport.sessionId() == 21);
    assert(transport.isOpen());

    std::vector<uint8_t> received(64, 0);
    const ssize_t n = ::recv(sockets[0], received.data(), received.size(), 0);
    assert(n > 0);
    received.resize(static_cast<size_t>(n));
    network::NetworkCommand command = {};
    assert(network::decodeFrame(received, command, error));
    assert(command.type == network::CommandType::Disconnect);
    assert(command.sessionId == 21);
    assert(command.payload.empty());

    const network::DisconnectResponse rejected = {
        network::CURRENT_PROTOCOL_VERSION,
        network::DisconnectResponseStatus::Rejected, 21,
        "disconnect command rate limit exceeded"};
    std::vector<uint8_t> rejectFrame;
    assert(network::encodeDisconnectResponseFrame(rejected, rejectFrame, error));
    assert(::send(sockets[0], rejectFrame.data(), rejectFrame.size(), 0) ==
           static_cast<ssize_t>(rejectFrame.size()));
    assert(transport.pump(error));
    assert(transport.authState() == client::TransportAuthState::LoggedIn);
    assert(transport.sessionId() == 21);
    assert(transport.isOpen());
    assert(transport.linkState() == client::TransportLinkState::Connected);

    assert(transport.enqueueDisconnect(error));
    assert(transport.pump(error));
    std::vector<uint8_t> ignored(64, 0);
    (void)::recv(sockets[0], ignored.data(), ignored.size(), 0);
    const network::DisconnectResponse acceptedAck = {
        network::CURRENT_PROTOCOL_VERSION,
        network::DisconnectResponseStatus::Accepted, 21, std::string()};
    std::vector<uint8_t> ackFrame;
    assert(network::encodeDisconnectResponseFrame(acceptedAck, ackFrame, error));
    assert(::send(sockets[0], ackFrame.data(), ackFrame.size(), 0) ==
           static_cast<ssize_t>(ackFrame.size()));
    assert(transport.pump(error));
    assert(transport.authState() == client::TransportAuthState::Anonymous);
    assert(transport.sessionId() == 0);
    assert(!transport.isOpen());
    assert(transport.linkState() == client::TransportLinkState::Disconnected);
    assert(transport.snapshotRequested());
    uint8_t eofPeek[8];
    const ssize_t peerClosed =
        ::recv(sockets[0], eofPeek, sizeof(eofPeek), MSG_DONTWAIT);
    assert(peerClosed == 0);
    ::close(sockets[0]);
}

void loopback_disconnect_ends_session_and_resets_client_auth() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("disconnect-session");
    server::ChallengeSessionLoginService auth(challenges, sessions, keys, clock);
    session::SessionRegistry registry;
    server::RegistryGameplaySessionPort gameplay(registry);
    server::ServerCommandDispatcher dispatcher(registry, auth, gameplay);
    server::ServerRuntime runtime;
    server::WorldFrameApplier applier(*Field::getInstance());
    assert(runtime.start(0));
    server::LoginFieldSpawnSettings spawn;
    spawn.playerName = "Hero";
    server::FieldSessionPresence::useSpawnSettings(spawn);
    const uint16_t portNum = runtime.listeningPort();
    assert(portNum != 0);

    const std::string challengeKey = "liss0152-disconnect";
    challenges.putUnclaimed(challengeKey, 9001, now + 120);

    client::ClientTransportShell transport;
    std::string error;
    assert(transport.connectTcp("127.0.0.1", portNum, error));
    assert(transport.enqueueLogin(challengeKey, error));
    assert(transport.flushOutbound(error) == server::SendStatus::Sent);

    server::ServerFrameResult frame = {};
    bool loggedIn = false;
    for (int i = 0; i < 8 && !loggedIn; ++i) {
        serverTick(runtime, dispatcher, applier, frame);
        assert(transport.pump(error));
        loggedIn = transport.authState() == client::TransportAuthState::LoggedIn;
    }
    assert(loggedIn);
    const int64_t sessionId = transport.sessionId();
    Field* field = Field::getInstance();
    const Player* placed = field->findPlayer(sessionId);
    assert(placed != 0);
    const int64_t gameplayId = placed->getPlayerId();
    assert(placed->getAuthPlayerId() == 9001);
    assert(!field->publicPlayerPoses().empty());

    assert(transport.enqueueDisconnect(error));
    assert(transport.pump(error));
    assert(transport.authState() == client::TransportAuthState::LoggedIn);
    assert(transport.sessionId() == sessionId);
    assert(transport.isOpen());

    for (int i = 0; i < 32 &&
                    transport.authState() == client::TransportAuthState::LoggedIn;
         ++i) {
        serverTick(runtime, dispatcher, applier, frame);
        assert(transport.pump(error));
    }
    assert(transport.authState() == client::TransportAuthState::Anonymous);
    assert(transport.sessionId() == 0);
    assert(!transport.isOpen());
    assert(transport.linkState() == client::TransportLinkState::Disconnected);
    assert(transport.snapshotRequested());
    assert(!registry.isActive(sessionId));
    assert(!field->hasPlayer(sessionId));
    assert(field->hasPlayer(gameplayId));
    assert(field->findPlayer(gameplayId)->getAuthPlayerId() == 9001);
    assert(field->publicPlayerPoses().empty());
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9002, "Hero"));

    transport.beginReconnect();
    assert(transport.snapshotRequested());
    challenges.putUnclaimed(challengeKey, 9001, now + 120);
    assert(transport.connectTcp("127.0.0.1", portNum, error));
    assert(transport.enqueueLogin(challengeKey, error));
    assert(transport.flushOutbound(error) == server::SendStatus::Sent);
    loggedIn = false;
    size_t snapshotRequestsSeen = 0;
    for (int i = 0; i < 32; ++i) {
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
    const Player* rebound = field->findPlayer(transport.sessionId());
    assert(rebound != 0);
    assert(rebound->getPlayerId() == gameplayId);
    assert(rebound->getAuthPlayerId() == 9001);
    assert(rebound->getPlayerName() == "Hero");

    assert(runtime.stop());
    resetFieldSessionPresence();
    clearFieldPlayers();
}

} // namespace client_transport_shell_tests

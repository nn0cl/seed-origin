#include <cassert>
#include <map>
#include <string>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "ChallengeAuthTestFixtures.h"
#include "TransportLoopbackTestSupport.h"
#include "ClientTransportShell.h"
#include "FieldSessionPresence.h"
#include "LoginFieldSpawnSettings.h"
#include "LoginResponseCodec.h"
#include "MonotonicClockPort.h"
#include "NetworkFrameCodec.h"
#include "RegistryGameplaySessionPort.h"
#include "ServerCommandDispatcher.h"
#include "ServerRuntime.h"
#include "SessionRegistry.h"
#include "WorldFrameApplier.h"
#include "TransportErrorReason.h"
#include "TransportTimeouts.h"
#include "WorldUpdateFrameCodec.h"

namespace client_transport_shell_timeout_tests {
namespace {

using seed_test::clearFieldPlayers;
using seed_test::FakeChallengeClaimPort;
using seed_test::FakeSessionStore;
using seed_test::FixedKeyIssuer;
using seed_test::FixedPlayerIdPort;
using seed_test::FixedWallClock;
using seed_test::resetFieldSessionPresence;
using seed_test::serverTick;

class FakeMonotonicClock : public client::MonotonicClockPort {
public:
    FakeMonotonicClock() : nowMs_(0) {}
    uint64_t nowMs() const override { return nowMs_; }
    void advance(uint64_t deltaMs) { nowMs_ += deltaMs; }

private:
    uint64_t nowMs_;
};

class SystemMonotonicClock : public client::MonotonicClockPort {
public:
    uint64_t nowMs() const override {
        struct timespec ts = {};
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return static_cast<uint64_t>(ts.tv_sec) * 1000ULL +
               static_cast<uint64_t>(ts.tv_nsec / 1000000ULL);
    }
};

}

void snapshot_wait_timeout_records_failure_and_keeps_snapshot_pending() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    FakeMonotonicClock clock;
    client::ClientTransportShell transport;
    transport.setClock(&clock);
    client::TransportTimeouts timeouts = {3000, 5000, 0};
    transport.setTimeouts(timeouts);
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
    assert(transport.authState() == client::TransportAuthState::LoggedIn);
    assert(transport.snapshotRequested());
    assert(transport.snapshotRequestCount() == 1);

    std::vector<uint8_t> requestBytes(64, 0);
    const ssize_t requestSize =
        ::recv(sockets[0], requestBytes.data(), requestBytes.size(), 0);
    assert(requestSize > 0);

    clock.advance(5001);
    assert(!transport.pump(error));
    assert(transport.linkState() == client::TransportLinkState::Failed);
    assert(transport.lastError() == client::TransportErrorReason::SnapshotWaitTimeout);
    assert(transport.lastErrorDetail() == "snapshot wait timed out");
    assert(transport.snapshotRequested());
    assert(transport.snapshotRequestCount() == 1);
    ::close(sockets[0]);
}

void login_response_wait_timeout_records_failure_during_reconnect() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    FakeMonotonicClock clock;
    client::ClientTransportShell transport;
    transport.setClock(&clock);
    client::TransportTimeouts timeouts = {3000, 5000, 0};
    transport.setTimeouts(timeouts);
    std::string error;
    transport.beginReconnect();
    assert(transport.attachConnectedSocket(sockets[1], error));
    assert(transport.pump(error));

    clock.advance(3001);
    assert(!transport.pump(error));
    assert(transport.linkState() == client::TransportLinkState::Failed);
    assert(transport.lastError() ==
           client::TransportErrorReason::LoginResponseTimeout);
    assert(transport.lastErrorDetail() == "login response wait timed out");
    assert(transport.authState() == client::TransportAuthState::Anonymous);
    ::close(sockets[0]);
}

void peer_close_records_peer_closed_on_logged_in_transport() {
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

    ::shutdown(sockets[0], SHUT_RDWR);
    ::close(sockets[0]);
    assert(!transport.pump(error));
    assert(transport.linkState() == client::TransportLinkState::Disconnected);
    assert(transport.lastError() == client::TransportErrorReason::PeerClosed);
    assert(transport.lastErrorDetail() == "peer closed connection");
    assert(transport.authState() == client::TransportAuthState::Anonymous);
}

void corrupt_inbound_frame_records_protocol_error() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    client::ClientTransportShell transport;
    std::string error;
    assert(transport.attachConnectedSocket(sockets[1], error));

    const uint8_t corrupt[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
    assert(::send(sockets[0], corrupt, sizeof(corrupt), 0) ==
           static_cast<ssize_t>(sizeof(corrupt)));
    assert(!transport.pump(error));
    assert(transport.linkState() == client::TransportLinkState::Failed);
    assert(transport.lastError() == client::TransportErrorReason::ProtocolError);
    assert(transport.lastErrorDetail() ==
           "protocol error: inbound frame prefix is invalid");
    ::close(sockets[0]);
}

void connect_timeout_records_connect_timeout() {
    SystemMonotonicClock clock;
    client::ClientTransportShell transport;
    transport.setClock(&clock);
    client::TransportTimeouts timeouts = {0, 0, 200};
    transport.setTimeouts(timeouts);
    std::string error;
    assert(!transport.connectTcp("198.18.0.1", 1, error));
    assert(transport.linkState() == client::TransportLinkState::Failed);
    assert(transport.lastError() == client::TransportErrorReason::ConnectTimeout);
    assert(transport.lastErrorDetail() == "connect timed out");
}

void successful_reconnect_increments_reconnect_counter() {
    clearFieldPlayers();

    const FixedPlayerIdPort playerIdPort(9001);
    server::FieldSessionPresence::usePlayerIdPort(&playerIdPort);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));

    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("timeout-session");
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

    const std::string challengeKey = "liss0128-timeout-ops";
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
    assert(transport.reconnectCount() == 0);
    assert(transport.lastError() == client::TransportErrorReason::None);

    transport.beginReconnect();
    challenges.putUnclaimed(challengeKey, 42, now + 120);
    assert(transport.connectTcp("127.0.0.1", port, error));
    assert(transport.enqueueLogin(challengeKey, error));
    assert(transport.flushOutbound(error) == server::SendStatus::Sent);
    loggedIn = false;
    for (int i = 0; i < 32; ++i) {
        serverTick(runtime, dispatcher, applier, frame);
        assert(transport.pump(error));
        if (transport.authState() == client::TransportAuthState::LoggedIn &&
            !transport.snapshotRequested()) {
            loggedIn = true;
            break;
        }
    }
    assert(loggedIn);
    assert(transport.reconnectCount() == 1);
    assert(transport.lastError() == client::TransportErrorReason::None);

    assert(runtime.stop());
    resetFieldSessionPresence();
    clearFieldPlayers();
}

} // namespace client_transport_shell_timeout_tests

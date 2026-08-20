#include "ClientTransportShell.h"

#include "DisconnectResponseCodec.h"
#include "LoginResponseCodec.h"
#include "NetworkFrameCodec.h"
#include "Protocol.h"
#include "WorldUpdateFrameCodec.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace client {

namespace {
constexpr std::size_t READ_BUFFER_SIZE = 4096;
}

ClientTransportShell::ClientTransportShell()
    : clientSocket(-1), link(TransportLinkState::Disconnected),
      auth(TransportAuthState::Anonymous), localSessionId(0),
      snapshotCommandQueued(false), disconnectQueued(false), outboundFrames(),
      inbound(), receiver(), clock(0), timeouts(), lastErrorReason(TransportErrorReason::None),
      lastErrorDetailText(), reconnectCounter(0), snapshotRequestCounter(0),
      reconnectPending(false), loginResponseDeadlineMs(0),
      snapshotWaitDeadlineMs(0) {}

ClientTransportShell::~ClientTransportShell() {
    close();
}

void ClientTransportShell::setClock(MonotonicClockPort* injectedClock) {
    clock = injectedClock;
}

void ClientTransportShell::setTimeouts(const TransportTimeouts& configured) {
    timeouts = configured;
}

TransportErrorReason ClientTransportShell::lastError() const {
    return lastErrorReason;
}

const std::string& ClientTransportShell::lastErrorDetail() const {
    return lastErrorDetailText;
}

uint32_t ClientTransportShell::reconnectCount() const { return reconnectCounter; }

uint32_t ClientTransportShell::snapshotRequestCount() const {
    return snapshotRequestCounter;
}

void ClientTransportShell::recordError(TransportErrorReason reason,
                                       const std::string& detail) {
    lastErrorReason = reason;
    lastErrorDetailText = detail;
}

void ClientTransportShell::clearWaitDeadlines() {
    loginResponseDeadlineMs = 0;
    snapshotWaitDeadlineMs = 0;
}

void ClientTransportShell::armLoginResponseWaitIfNeeded() {
    if (clock == 0 || timeouts.loginResponseWaitMs == 0) return;
    if (!reconnectPending || auth != TransportAuthState::Anonymous) return;
    loginResponseDeadlineMs = clock->nowMs() + timeouts.loginResponseWaitMs;
}

void ClientTransportShell::armSnapshotWaitIfNeeded() {
    if (clock == 0 || timeouts.snapshotWaitMs == 0) return;
    if (auth != TransportAuthState::LoggedIn || !receiver.snapshotRequested()) {
        return;
    }
    snapshotWaitDeadlineMs = clock->nowMs() + timeouts.snapshotWaitMs;
}

void ClientTransportShell::noteSnapshotRequestQueued() {
    ++snapshotRequestCounter;
    armSnapshotWaitIfNeeded();
}

void ClientTransportShell::noteReconnectCompletedIfReady() {
    if (!reconnectPending || auth != TransportAuthState::LoggedIn ||
        receiver.snapshotRequested()) {
        return;
    }
    ++reconnectCounter;
    reconnectPending = false;
    recordError(TransportErrorReason::None, std::string());
}

bool ClientTransportShell::checkDeadlines(std::string& error) {
    if (clock == 0) return true;
    const uint64_t nowMs = clock->nowMs();
    if (loginResponseDeadlineMs > 0 && nowMs >= loginResponseDeadlineMs) {
        markFailed(TransportErrorReason::LoginResponseTimeout,
                   "login response wait timed out");
        error = lastErrorDetailText;
        return false;
    }
    if (snapshotWaitDeadlineMs > 0 && auth == TransportAuthState::LoggedIn &&
        receiver.snapshotRequested() && nowMs >= snapshotWaitDeadlineMs) {
        markFailed(TransportErrorReason::SnapshotWaitTimeout,
                   "snapshot wait timed out");
        error = lastErrorDetailText;
        return false;
    }
    return true;
}

bool ClientTransportShell::setNonBlocking(std::string& error) {
    const int flags = fcntl(clientSocket, F_GETFL, 0);
    if (flags < 0 || fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) != 0) {
        error = "client socket could not be set non-blocking";
        markFailed(TransportErrorReason::ProtocolError, error);
        return false;
    }
    return true;
}

bool ClientTransportShell::connectTcp(const char* host, uint16_t port,
                                      std::string& error) {
    if (clientSocket >= 0) {
        error = "client transport is already connected";
        return false;
    }
    if (host == 0 || port == 0) {
        error = "client transport address is invalid";
        return false;
    }

    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error = "client socket create failed";
        link = TransportLinkState::Failed;
        return false;
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        ::close(fd);
        error = "client transport host is not a usable IPv4 address";
        link = TransportLinkState::Failed;
        return false;
    }
    if (::connect(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) !=
        0) {
        ::close(fd);
        error = "client tcp connect failed";
        link = TransportLinkState::Failed;
        return false;
    }
    clientSocket = fd;
    if (!setNonBlocking(error)) return false;
    link = TransportLinkState::Connected;
    armLoginResponseWaitIfNeeded();
    error.clear();
    return true;
}

bool ClientTransportShell::attachConnectedSocket(int socketFd, std::string& error) {
    if (clientSocket >= 0) {
        error = "client transport is already connected";
        return false;
    }
    if (socketFd < 0) {
        error = "client transport socket is invalid";
        return false;
    }
    clientSocket = socketFd;
    if (!setNonBlocking(error)) return false;
    link = TransportLinkState::Connected;
    armLoginResponseWaitIfNeeded();
    error.clear();
    return true;
}

void ClientTransportShell::beginReconnect() {
    close();
    inbound.reset();
    outboundFrames.clear();
    auth = TransportAuthState::Anonymous;
    localSessionId = 0;
    snapshotCommandQueued = false;
    disconnectQueued = false;
    clearWaitDeadlines();
    reconnectPending = true;
    receiver.beginReconnect();
}

bool ClientTransportShell::enqueueLogin(const std::string& claimedId,
                                        std::string& error) {
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        claimedId};
    return enqueueCommand(command, error);
}

bool ClientTransportShell::enqueueDisconnect(std::string& error) {
    if (auth != TransportAuthState::LoggedIn) {
        error = "disconnect requires a logged-in session";
        return false;
    }
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Disconnect,
        localSessionId, ""};
    if (!enqueueCommand(command, error)) return false;
    disconnectQueued = true;
    return true;
}

bool ClientTransportShell::enqueueCommand(const network::NetworkCommand& command,
                                          std::string& error) {
    if (!isOpen()) {
        error = "client transport is closed";
        return false;
    }
    std::vector<uint8_t> frame;
    if (!network::encodeFrame(command, frame, error)) return false;
    return outboundFrames.enqueue(frame, error);
}

server::SendStatus ClientTransportShell::flushOutbound(std::string& error) {
    if (!isOpen()) {
        error = "client transport is closed";
        return server::SendStatus::Closed;
    }

    std::vector<uint8_t> frame;
    if (!outboundFrames.front(frame)) return server::SendStatus::NoData;
    const ssize_t sent = ::send(clientSocket, frame.data(), frame.size(), 0);
    if (sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            return server::SendStatus::NoData;
        }
        if (errno == ECONNRESET || errno == EPIPE) {
            recordError(TransportErrorReason::PeerClosed, "client transport peer closed");
            close();
            return server::SendStatus::Closed;
        }
        error = "client socket write failed";
        markFailed(TransportErrorReason::WriteFailed, error);
        return server::SendStatus::Failed;
    }
    if (sent == 0) return server::SendStatus::NoData;
    if (!outboundFrames.consumeFront(static_cast<size_t>(sent))) {
        error = "outbound frame progress is invalid";
        markFailed(TransportErrorReason::WriteFailed, error);
        return server::SendStatus::Failed;
    }
    error.clear();
    return server::SendStatus::Sent;
}

bool ClientTransportShell::enqueueRequestSnapshotIfNeeded(std::string& error) {
    if (snapshotCommandQueued || auth != TransportAuthState::LoggedIn) return true;
    network::NetworkCommand command = {};
    if (!receiver.tryBuildRequestSnapshotCommand(command)) return true;
    if (!enqueueCommand(command, error)) return false;
    snapshotCommandQueued = true;
    noteSnapshotRequestQueued();
    return true;
}

bool ClientTransportShell::handleLoginResponseFrame(
    const std::vector<uint8_t>& bytes, std::string& error) {
    network::LoginResponse response = {};
    if (!network::decodeLoginResponseFrame(bytes, response, error)) {
        markFailed(TransportErrorReason::ProtocolError, error);
        return false;
    }
    if (response.status != network::LoginResponseStatus::Accepted) {
        error = response.payload.empty() ? "login was rejected" : response.payload;
        markFailed(TransportErrorReason::LoginRejected, error);
        return false;
    }
    localSessionId = response.sessionId;
    auth = TransportAuthState::LoggedIn;
    receiver.bindLocalSession(localSessionId);
    loginResponseDeadlineMs = 0;
    armSnapshotWaitIfNeeded();
    return true;
}

bool ClientTransportShell::handleDisconnectResponseFrame(
    const std::vector<uint8_t>& bytes, std::string& error) {
    network::DisconnectResponse response = {};
    if (!network::decodeDisconnectResponseFrame(bytes, response, error)) {
        markFailed(TransportErrorReason::ProtocolError, error);
        return false;
    }
    applyDisconnectResponse(response);
    if (!isOpen()) {
        error.clear();
        return true;
    }
    return true;
}

bool ClientTransportShell::handleInboundFrames(
    const std::vector<InboundFrame>& frames, std::string& error) {
    std::vector<uint8_t> worldBytes;
    bool disconnectAccepted = false;
    for (std::vector<InboundFrame>::const_iterator it = frames.begin();
         it != frames.end(); ++it) {
        if (it->kind == InboundFrameKind::LoginResponse) {
            if (!handleLoginResponseFrame(it->bytes, error)) return false;
            continue;
        }
        if (it->kind == InboundFrameKind::DisconnectResponse) {
            if (!handleDisconnectResponseFrame(it->bytes, error)) return false;
            if (reconnectPending) disconnectAccepted = true;
            continue;
        }
        worldBytes.insert(worldBytes.end(), it->bytes.begin(), it->bytes.end());
    }
    if (disconnectAccepted || worldBytes.empty()) {
        error.clear();
        return true;
    }
    size_t applied = 0;
    if (!receiver.receive(worldBytes, applied, error) &&
        receiver.lastDecision() != WorldReceiveDecision::RequestSnapshot) {
        markFailed(TransportErrorReason::ProtocolError, error);
        return false;
    }
    if (!receiver.snapshotRequested()) {
        snapshotCommandQueued = false;
        snapshotWaitDeadlineMs = 0;
        noteReconnectCompletedIfReady();
    }
    error.clear();
    return true;
}

bool ClientTransportShell::pump(std::string& error) {
    if (inbound.failed() || link == TransportLinkState::Failed) {
        error = "client transport is failed";
        return false;
    }
    if (!isOpen()) {
        error = "client transport is closed";
        return false;
    }
    if (!checkDeadlines(error)) return false;

    uint8_t bytes[READ_BUFFER_SIZE];
    const ssize_t received = ::recv(clientSocket, bytes, sizeof(bytes), 0);
    if (received == 0) {
        recordError(TransportErrorReason::PeerClosed, "client transport peer closed");
        error = lastErrorDetailText;
        close();
        return false;
    }
    if (received < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            if (errno == ECONNRESET || errno == EPIPE) {
                recordError(TransportErrorReason::PeerClosed,
                            "client transport peer closed");
                error = lastErrorDetailText;
                close();
                return false;
            }
            error = "client socket read failed";
            markFailed(TransportErrorReason::ReadFailed, error);
            return false;
        }
    }
    if (received > 0) {
        if (!inbound.hasBufferedData() && received >= 2) {
            const uint16_t prefix =
                static_cast<uint16_t>((bytes[0] << 8) | bytes[1]);
            if (prefix != network::WORLD_UPDATE_FRAME_MAGIC &&
                prefix != network::CURRENT_PROTOCOL_VERSION) {
                error = "inbound frame prefix is invalid";
                markFailed(TransportErrorReason::ProtocolError, error);
                return false;
            }
        }
        const std::vector<uint8_t> input(bytes, bytes + received);
        std::vector<InboundFrame> frames;
        if (!inbound.append(input, frames, error)) {
            markFailed(TransportErrorReason::ProtocolError, error);
            return false;
        }
        if (!handleInboundFrames(frames, error)) return false;
    }
    if (!checkDeadlines(error)) return false;
    if (!enqueueRequestSnapshotIfNeeded(error)) return false;
    while (!outboundFrames.empty()) {
        const server::SendStatus status = flushOutbound(error);
        if (status == server::SendStatus::NoData) break;
        if (status != server::SendStatus::Sent) return false;
    }
    noteReconnectCompletedIfReady();
    error.clear();
    return true;
}

bool ClientTransportShell::close() {
    if (clientSocket < 0) {
        if (link != TransportLinkState::Failed) {
            link = TransportLinkState::Disconnected;
        }
        return true;
    }
    ::shutdown(clientSocket, SHUT_RDWR);
    const bool closed = (::close(clientSocket) == 0);
    clientSocket = -1;
    if (link != TransportLinkState::Failed) {
        link = TransportLinkState::Disconnected;
    }
    auth = TransportAuthState::Anonymous;
    return closed;
}

TransportLinkState ClientTransportShell::linkState() const { return link; }

TransportAuthState ClientTransportShell::authState() const { return auth; }

bool ClientTransportShell::isOpen() const { return clientSocket >= 0; }

int64_t ClientTransportShell::sessionId() const { return localSessionId; }

bool ClientTransportShell::snapshotRequested() const {
    return receiver.snapshotRequested();
}

const ClientWorldUpdateReceiver& ClientTransportShell::worldReceiver() const {
    return receiver;
}

void ClientTransportShell::applyDisconnectResponse(
    const network::DisconnectResponse& response) {
    if (!disconnectQueued) return;
    if (response.status == network::DisconnectResponseStatus::Rejected) {
        disconnectQueued = false;
        return;
    }
    if (response.sessionId != localSessionId) return;
    beginReconnect();
}

void ClientTransportShell::markFailed(TransportErrorReason reason,
                                      const std::string& detail) {
    recordError(reason, detail);
    if (clientSocket >= 0) {
        ::close(clientSocket);
        clientSocket = -1;
    }
    link = TransportLinkState::Failed;
    auth = TransportAuthState::Anonymous;
    disconnectQueued = false;
    clearWaitDeadlines();
}

}

#include "ClientSession.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace server {

namespace {
constexpr std::size_t READ_BUFFER_SIZE = 4096;
}

ClientSession::ClientSession(int clientSocket)
    : clientSocket(clientSocket), accumulator(), outboundFrames() {
    if (this->clientSocket < 0) return;
    const int flags = fcntl(this->clientSocket, F_GETFL, 0);
    if (flags < 0 || fcntl(this->clientSocket, F_SETFL, flags | O_NONBLOCK) != 0) {
        ::close(this->clientSocket);
        this->clientSocket = -1;
    }
}

ClientSession::~ClientSession() {
    close();
}

ReceiveStatus ClientSession::receive(std::vector<network::NetworkCommand>& commands,
                                     std::string& error) {
    commands.clear();
    if (clientSocket < 0) {
        error = "client session is closed";
        return ReceiveStatus::Closed;
    }

    uint8_t bytes[READ_BUFFER_SIZE];
    const ssize_t received = ::recv(clientSocket, bytes, sizeof(bytes), 0);
    if (received == 0) {
        close();
        return ReceiveStatus::Closed;
    }
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return ReceiveStatus::NoData;
        error = "client socket read failed";
        close();
        return ReceiveStatus::Failed;
    }

    const std::vector<uint8_t> input(bytes, bytes + received);
    if (!accumulator.append(input, commands, error)) {
        close();
        return ReceiveStatus::Failed;
    }
    return commands.empty() ? ReceiveStatus::NoData : ReceiveStatus::Commands;
}

bool ClientSession::enqueueFrame(const std::vector<uint8_t>& frame, std::string& error) {
    if (!isOpen()) {
        error = "client session is closed";
        return false;
    }
    return outboundFrames.enqueue(frame, error);
}

SendStatus ClientSession::flushOutbound(std::string& error) {
    if (!isOpen()) {
        error = "client session is closed";
        return SendStatus::Closed;
    }

    std::vector<uint8_t> frame;
    if (!outboundFrames.front(frame)) return SendStatus::NoData;
    const ssize_t sent = ::send(clientSocket, frame.data(), frame.size(), 0);
    if (sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return SendStatus::NoData;
        error = "client socket write failed";
        close();
        return SendStatus::Failed;
    }
    if (sent == 0) return SendStatus::NoData;
    if (!outboundFrames.consumeFront(static_cast<size_t>(sent))) {
        error = "outbound frame progress is invalid";
        close();
        return SendStatus::Failed;
    }
    error.clear();
    return SendStatus::Sent;
}

bool ClientSession::close() {
    if (clientSocket < 0) return true;
    const bool closed = (::close(clientSocket) == 0);
    clientSocket = -1;
    return closed;
}

bool ClientSession::isOpen() const {
    return clientSocket >= 0;
}

}

#include "ClientSession.h"

#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

namespace server {

namespace {
static const size_t READ_BUFFER_SIZE = 4096;
}

ClientSession::ClientSession(int clientSocket)
    : clientSocket(clientSocket), accumulator() {}

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

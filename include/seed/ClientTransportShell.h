#ifndef SEED_CLIENT_TRANSPORT_SHELL_H
#define SEED_CLIENT_TRANSPORT_SHELL_H

#include <cstdint>
#include <string>
#include <vector>

#include "ClientInboundDemux.h"
#include "ClientSession.h"
#include "ClientWorldUpdateReceiver.h"
#include "DisconnectResponse.h"
#include "OutboundFrameQueue.h"

namespace client {

enum class TransportLinkState {
    Disconnected,
    Connected,
    Failed
};

enum class TransportAuthState {
    Anonymous,
    LoggedIn
};

class ClientTransportShell {
public:
    ClientTransportShell();
    ~ClientTransportShell();

    ClientTransportShell(const ClientTransportShell&) = delete;
    ClientTransportShell& operator=(const ClientTransportShell&) = delete;

    bool connectTcp(const char* host, uint16_t port, std::string& error);
    bool attachConnectedSocket(int socketFd, std::string& error);
    void beginReconnect();
    bool enqueueLogin(const std::string& claimedId, std::string& error);
    bool enqueueDisconnect(std::string& error);
    bool enqueueCommand(const network::NetworkCommand& command, std::string& error);
    server::SendStatus flushOutbound(std::string& error);
    bool pump(std::string& error);
    bool close();

    TransportLinkState linkState() const;
    TransportAuthState authState() const;
    bool isOpen() const;
    int64_t sessionId() const;
    bool snapshotRequested() const;
    const ClientWorldUpdateReceiver& worldReceiver() const;

private:
    int clientSocket;
    TransportLinkState link;
    TransportAuthState auth;
    int64_t localSessionId;
    bool snapshotCommandQueued;
    bool disconnectQueued;
    server::OutboundFrameQueue outboundFrames;
    ClientInboundDemux inbound;
    ClientWorldUpdateReceiver receiver;

    bool setNonBlocking(std::string& error);
    bool enqueueRequestSnapshotIfNeeded(std::string& error);
    bool handleInboundFrames(const std::vector<InboundFrame>& frames,
                             std::string& error);
    bool handleLoginResponseFrame(const std::vector<uint8_t>& bytes,
                                  std::string& error);
    bool handleDisconnectResponseFrame(const std::vector<uint8_t>& bytes,
                                       std::string& error);
    void applyDisconnectResponse(const network::DisconnectResponse& response);
    void markFailed();
};

}

#endif

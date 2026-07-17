#ifndef SEED_CLIENT_SESSION_H
#define SEED_CLIENT_SESSION_H

#include <string>
#include <vector>

#include "FrameAccumulator.h"
#include "OutboundFrameQueue.h"

namespace server {

enum class ReceiveStatus {
    Commands,
    NoData,
    Closed,
    Failed
};

enum class SendStatus {
    Sent,
    NoData,
    Closed,
    Failed
};

class ClientSession {
public:
    explicit ClientSession(int clientSocket);
    ~ClientSession();

    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;

    ReceiveStatus receive(std::vector<network::NetworkCommand>& commands,
                          std::string& error);
    bool enqueueFrame(const std::vector<uint8_t>& frame, std::string& error);
    SendStatus flushOutbound(std::string& error);
    bool close();
    bool isOpen() const;

private:
    int clientSocket;
    network::FrameAccumulator accumulator;
    OutboundFrameQueue outboundFrames;
};

}

#endif

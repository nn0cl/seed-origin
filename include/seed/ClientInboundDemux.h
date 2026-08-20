#ifndef SEED_CLIENT_INBOUND_DEMUX_H
#define SEED_CLIENT_INBOUND_DEMUX_H

#include <cstdint>
#include <string>
#include <vector>

namespace client {

enum class InboundFrameKind {
    LoginResponse,
    DisconnectResponse,
    WorldUpdate
};

struct InboundFrame {
    InboundFrameKind kind;
    std::vector<uint8_t> bytes;
};

class ClientInboundDemux {
public:
    ClientInboundDemux();
    bool append(const std::vector<uint8_t>& bytes,
                std::vector<InboundFrame>& frames,
                std::string& error);
    void reset();
    bool failed() const;
    bool hasBufferedData() const;

private:
    std::vector<uint8_t> buffered;
    bool failedState;
};

}

#endif

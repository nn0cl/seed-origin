#include "TransportErrorDetail.h"

namespace client {

std::string formatTransportErrorDetail(TransportErrorReason reason,
                                       const std::string& context) {
    switch (reason) {
    case TransportErrorReason::None:
        return std::string();
    case TransportErrorReason::ConnectTimeout:
        return "connect timed out";
    case TransportErrorReason::LoginResponseTimeout:
        return "login response wait timed out";
    case TransportErrorReason::SnapshotWaitTimeout:
        return "snapshot wait timed out";
    case TransportErrorReason::PeerClosed:
        return "peer closed connection";
    case TransportErrorReason::ReadFailed:
        return context.empty() ? "read failed" : "read failed: " + context;
    case TransportErrorReason::WriteFailed:
        return context.empty() ? "write failed" : "write failed: " + context;
    case TransportErrorReason::ProtocolError:
        return context.empty() ? "protocol error" : "protocol error: " + context;
    case TransportErrorReason::LoginRejected:
        return context.empty() ? "login rejected" : "login rejected: " + context;
    }
    return context;
}

}

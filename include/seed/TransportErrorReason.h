#ifndef SEED_TRANSPORT_ERROR_REASON_H
#define SEED_TRANSPORT_ERROR_REASON_H

namespace client {

enum class TransportErrorReason {
    None,
    ConnectTimeout,
    LoginResponseTimeout,
    SnapshotWaitTimeout,
    PeerClosed,
    ReadFailed,
    WriteFailed,
    ProtocolError,
    LoginRejected
};

}

#endif

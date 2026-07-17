#include "WorldUpdate.h"

#include "NetworkCommand.h"

namespace network {

namespace {

bool isKnownUpdateKind(UpdateKind kind) {
    return kind == UpdateKind::Snapshot || kind == UpdateKind::Event;
}

}

bool validateWorldUpdate(const WorldUpdate& update, std::string& error) {
    if (update.version != CURRENT_PROTOCOL_VERSION) {
        error = "unsupported update protocol version";
        return false;
    }
    if (!isKnownUpdateKind(update.kind)) {
        error = "unknown world update kind";
        return false;
    }
    if (update.sequence == 0) {
        error = "world update sequence must be positive";
        return false;
    }
    if (update.payload.size() > MAX_UPDATE_PAYLOAD) {
        error = "world update payload too large";
        return false;
    }
    if (update.kind == UpdateKind::Snapshot && update.eventId != 0) {
        error = "snapshot must not provide an event id";
        return false;
    }
    if (update.kind == UpdateKind::Event && update.eventId == 0) {
        error = "event must provide an event id";
        return false;
    }
    error.clear();
    return true;
}

}

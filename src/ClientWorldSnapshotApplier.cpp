#include "ClientWorldSnapshotApplier.h"

#include <cmath>
#include <limits>
#include <sstream>
#include <string>

namespace client {
namespace {

bool parseFloat(const std::string& text, float& value) {
    std::istringstream input(text);
    input >> value;
    std::string trailing;
    if (!input || (input >> trailing) || !std::isfinite(value)) return false;
    return true;
}

bool parsePayload(const std::string& payload, EnvironmentState& state,
                 std::string& error) {
    std::istringstream fields(payload);
    std::string field;
    bool present[5] = {false, false, false, false, false};
    float* values[5] = {&state.fire, &state.water, &state.earth,
                        &state.air, &state.hazard};
    const char* keys[5] = {"ether.fire", "ether.water", "ether.earth",
                           "ether.air", "ether.hazard"};
    while (std::getline(fields, field, ';')) {
        const std::string::size_type separator = field.find('=');
        if (separator == std::string::npos) {
            error = "snapshot payload contains a field without a value";
            return false;
        }
        const std::string key = field.substr(0, separator);
        int index = -1;
        for (int i = 0; i < 5; ++i) {
            if (key == keys[i]) index = i;
        }
        if (index < 0 || present[index] ||
            !parseFloat(field.substr(separator + 1), *values[index])) {
            error = "snapshot payload contains an invalid or duplicate field";
            return false;
        }
        present[index] = true;
    }
    for (int i = 0; i < 5; ++i) {
        if (!present[i]) {
            error = "snapshot payload is missing an environment field";
            return false;
        }
    }
    if (state.hazard < 0.0f || state.hazard > 10.0f) {
        error = "snapshot hazard is outside the supported range";
        return false;
    }
    return true;
}

}

ClientWorldSnapshotApplier::ClientWorldSnapshotApplier() : sync() {}

bool ClientWorldSnapshotApplier::applySnapshot(
    const network::WorldUpdate& update, ClientEnvironmentState& state,
    std::string& error) {
    if (update.kind != network::UpdateKind::Snapshot || update.eventId != 0) {
        error = "only a zero-event snapshot can replace client state";
        return false;
    }
    if (!network::validateWorldUpdate(update, error)) return false;
    if (update.sequence == std::numeric_limits<uint64_t>::max()) {
        error = "snapshot sequence cannot be incremented";
        return false;
    }
    EnvironmentState next = {};
    if (!parsePayload(update.payload, next, error)) return false;
    next.worldTick = update.worldTick;
    next.sequence = update.sequence;
    state.replace(next);
    sync.confirmSnapshot(update.sequence);
    error.clear();
    return true;
}

const network::WorldUpdateSyncController&
ClientWorldSnapshotApplier::synchronization() const {
    return sync;
}

}

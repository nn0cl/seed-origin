#include "ClientWorldUpdateReceiver.h"

namespace client {

ClientWorldUpdateReceiver::ClientWorldUpdateReceiver()
    : accumulator(), snapshotApplier(), environmentState(), hazardQueue() {}

bool ClientWorldUpdateReceiver::receive(const std::vector<uint8_t>& bytes,
                                        size_t& applied,
                                        std::string& error) {
    applied = 0;
    std::vector<network::WorldUpdate> updates;
    if (!accumulator.append(bytes, updates, error)) return false;
    for (std::vector<network::WorldUpdate>::const_iterator it = updates.begin();
         it != updates.end(); ++it) {
        if (it->kind == network::UpdateKind::Snapshot) {
            if (!snapshotApplier.applySnapshot(*it, environmentState, error))
                return false;
            hazardQueue.confirmSnapshot(it->sequence);
        } else if (it->kind == network::UpdateKind::Event) {
            const HazardIngestResult result = hazardQueue.ingest(*it, error);
            if (result != HazardIngestResult::Applied &&
                result != HazardIngestResult::IgnoredDuplicate) {
                return false;
            }
        } else {
            error = "world update kind is unsupported";
            return false;
        }
        ++applied;
    }
    error.clear();
    return true;
}

const ClientEnvironmentState& ClientWorldUpdateReceiver::environment() const {
    return environmentState;
}

const ClientHazardEffectQueue& ClientWorldUpdateReceiver::hazardEffects() const {
    return hazardQueue;
}

}

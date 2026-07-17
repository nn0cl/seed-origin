#include "ClientWorldUpdateReceiver.h"

namespace client {

ClientWorldUpdateReceiver::ClientWorldUpdateReceiver()
    : accumulator(), snapshotApplier(), environmentState(), hazardQueue(),
      needsSnapshot(false), expected(1), decision(WorldReceiveDecision::NoData) {}

bool ClientWorldUpdateReceiver::receive(const std::vector<uint8_t>& bytes,
                                        size_t& applied,
                                        std::string& error) {
    applied = 0;
    decision = WorldReceiveDecision::NoData;
    std::vector<network::WorldUpdate> updates;
    if (!accumulator.append(bytes, updates, error)) {
        decision = WorldReceiveDecision::Rejected;
        return false;
    }
    if (updates.empty()) {
        error.clear();
        return true;
    }
    for (std::vector<network::WorldUpdate>::const_iterator it = updates.begin();
         it != updates.end(); ++it) {
        if (it->kind == network::UpdateKind::Snapshot) {
            if (!snapshotApplier.applySnapshot(*it, environmentState, error))
                { decision = WorldReceiveDecision::Rejected; return false; }
            hazardQueue.confirmSnapshot(it->sequence);
            needsSnapshot = false;
            expected = it->sequence + 1;
        } else if (it->kind == network::UpdateKind::Event) {
            if (needsSnapshot) {
                error = "world update cannot be applied before snapshot resync";
                decision = WorldReceiveDecision::RequestSnapshot;
                return false;
            }
            const HazardIngestResult result = hazardQueue.ingest(*it, error);
            expected = hazardQueue.synchronization().expectedSequence();
            if (result == HazardIngestResult::RequestSnapshot) {
                needsSnapshot = true;
                decision = WorldReceiveDecision::RequestSnapshot;
                return false;
            }
            if (result != HazardIngestResult::Applied &&
                result != HazardIngestResult::IgnoredDuplicate) {
                decision = WorldReceiveDecision::Rejected;
                return false;
            }
        } else {
            error = "world update kind is unsupported";
            decision = WorldReceiveDecision::Rejected;
            return false;
        }
        ++applied;
    }
    decision = WorldReceiveDecision::Applied;
    error.clear();
    return true;
}

void ClientWorldUpdateReceiver::beginReconnect() {
    accumulator.reset();
    needsSnapshot = true;
    expected = 1;
    decision = WorldReceiveDecision::RequestSnapshot;
}

bool ClientWorldUpdateReceiver::snapshotRequested() const { return needsSnapshot; }

uint64_t ClientWorldUpdateReceiver::expectedSequence() const { return expected; }

WorldReceiveDecision ClientWorldUpdateReceiver::lastDecision() const {
    return decision;
}

const ClientEnvironmentState& ClientWorldUpdateReceiver::environment() const {
    return environmentState;
}

const ClientHazardEffectQueue& ClientWorldUpdateReceiver::hazardEffects() const {
    return hazardQueue;
}

}

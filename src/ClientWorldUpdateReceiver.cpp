#include "ClientWorldUpdateReceiver.h"

#include "ClientInputSequence.h"

namespace client {

ClientWorldUpdateReceiver::ClientWorldUpdateReceiver()
    : accumulator(), snapshotApplier(), environmentState(), hazardQueue(),
      predictor(), remotes(), localSessionId(0), needsSnapshot(false), expected(1),
      decision(WorldReceiveDecision::NoData) {}

bool ClientWorldUpdateReceiver::applyEvent(const network::WorldUpdate& update,
                                           std::string& error) {
    if (needsSnapshot) {
        error = "world update cannot be applied before snapshot resync";
        decision = WorldReceiveDecision::RequestSnapshot;
        return false;
    }
    const bool hazard = update.payload.compare(0, 12, "etherHazard=") == 0;
    const bool movementAckEvent =
        update.payload.compare(0, 12, "movementAck=") == 0;
    HazardIngestResult result = HazardIngestResult::Rejected;
    if (hazard) {
        result = hazardQueue.ingest(update, error);
    } else {
        result = hazardQueue.observeEvent(update, error);
    }
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
    if (movementAckEvent && result == HazardIngestResult::Applied) {
        server::MovementAck ack;
        if (!server::parseMovementAck(update.payload, ack, error)) {
            decision = WorldReceiveDecision::Rejected;
            return false;
        }
        if (localSessionId > 0 && ack.sessionId == localSessionId) {
            predictor.reconcile(ack.x, ack.y, ack.z, ack.worldTick,
                                ack.lastProcessedInputSequence);
        }
    } else if (result == HazardIngestResult::Applied) {
        server::MovementAck ack;
        if (server::tryParseAttachedMovementAck(update.payload, ack) &&
            localSessionId > 0 && ack.sessionId == localSessionId) {
            predictor.reconcile(ack.x, ack.y, ack.z, ack.worldTick,
                                ack.lastProcessedInputSequence);
        }
        server::PublicMovementPose publicPose;
        if (server::tryParsePublicMovementPose(update.payload, publicPose) &&
            (localSessionId <= 0 || publicPose.sessionId != localSessionId)) {
            remotes.applyAuthoritative(publicPose.sessionId, publicPose.x,
                                       publicPose.y, publicPose.z, false);
        }
    }
    return true;
}

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
            if (environmentState.value().hasLocalPlayer) {
                predictor.applySnapshotBaseline(
                    environmentState.value().localX,
                    environmentState.value().localY,
                    environmentState.value().localZ,
                    it->worldTick,
                    environmentState.value().lastProcessedInputSequence);
            }
            remotes.replaceFromSnapshot(environmentState.value().players,
                                        localSessionId);
        } else if (it->kind == network::UpdateKind::Event) {
            if (needsSnapshot) {
                continue;
            }
            if (!applyEvent(*it, error)) return false;
        } else {
            error = "world update kind is unsupported";
            decision = WorldReceiveDecision::Rejected;
            return false;
        }
        ++applied;
    }
    if (needsSnapshot) {
        error = "world update cannot be applied before snapshot resync";
        decision = WorldReceiveDecision::RequestSnapshot;
        return false;
    }
    decision = WorldReceiveDecision::Applied;
    error.clear();
    return true;
}

void ClientWorldUpdateReceiver::bindLocalSession(int64_t sessionId) {
    localSessionId = sessionId;
}

void ClientWorldUpdateReceiver::beginReconnect() {
    accumulator.reset();
    needsSnapshot = true;
    expected = 1;
    decision = WorldReceiveDecision::RequestSnapshot;
}

bool ClientWorldUpdateReceiver::tryBuildRequestSnapshotCommand(
    network::NetworkCommand& command) const {
    if (!needsSnapshot || localSessionId <= 0) return false;
    command = network::makeRequestSnapshotCommand(localSessionId);
    return true;
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

LocalMovementPredictor& ClientWorldUpdateReceiver::localMovement() {
    return predictor;
}

const LocalMovementPredictor& ClientWorldUpdateReceiver::localMovement() const {
    return predictor;
}

const RemotePlayerPoseStore& ClientWorldUpdateReceiver::remotePlayers() const {
    return remotes;
}

}

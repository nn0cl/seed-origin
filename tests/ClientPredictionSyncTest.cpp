#include <cassert>
#include <cmath>
#include <string>
#include <vector>

#include "ClientWorldUpdateReceiver.h"
#include "Field.h"
#include "LocalMovementPredictor.h"
#include "Player.h"
#include "Status.h"
#include "WorldFrameApplier.h"
#include "WorldFrameUpdateBuilder.h"
#include "WorldUpdateFrameCodec.h"

namespace client_prediction_sync_tests {
namespace {

network::WorldUpdate snapshotWithLocal(uint64_t sequence, uint64_t worldTick,
                                       float x, uint64_t lastProcessed) {
    return {1, network::UpdateKind::Snapshot, worldTick, sequence, 0,
            "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
            "local.x=" +
                std::to_string(x) + ";local.y=0;local.z=0;"
                "local.lastProcessedInputSequence=" +
                std::to_string(lastProcessed)};
}

network::WorldUpdate movementAck(uint64_t sequence, uint64_t eventId,
                                 uint64_t worldTick, int64_t sessionId, float x,
                                 uint64_t lastProcessed) {
    return {1, network::UpdateKind::Event, worldTick, sequence, eventId,
            "movementAck=session:" + std::to_string(sessionId) +
                ";x:" + std::to_string(x) + ";y:0;z:0;worldTick:" +
                std::to_string(worldTick) + ";lastProcessedInputSequence:" +
                std::to_string(lastProcessed)};
}

network::WorldUpdate publicMovement(uint64_t sequence, uint64_t eventId,
                                    uint64_t worldTick, int64_t sessionId) {
    return {1, network::UpdateKind::Event, worldTick, sequence, eventId,
            "movement=session:" + std::to_string(sessionId) +
                ";dx=1;dy=0;dz=0;clientInputSequence=1;inputSequence=1"};
}

network::WorldUpdate publicPlayerSnapshot(uint64_t sequence, uint64_t worldTick,
                                          int64_t sessionId, int64_t gameplayId,
                                          float x, const std::string& name) {
    std::string payload =
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "player.count=1;player.0.session=" +
        std::to_string(sessionId) + ";player.0.x=" + std::to_string(x) +
        ";player.0.y=0;player.0.z=0;player.0.id=" + std::to_string(gameplayId) +
        ";player.0.name=" + name;
    return {1, network::UpdateKind::Snapshot, worldTick, sequence, 0, payload};
}

network::WorldUpdate hazard(uint64_t sequence, uint64_t eventId) {
    return {1, network::UpdateKind::Event, 8, sequence, eventId,
            "etherHazard=severity:2;instability:3"};
}

bool encode(const network::WorldUpdate& update, std::vector<uint8_t>& bytes) {
    std::string error;
    return network::encodeWorldUpdateFrame(update, bytes, error);
}

Player namedResident(int64_t playerId, float x, const char* name) {
    Player player(playerId, Status(), Position(playerId, x, 0.0f, 0.0f));
    assert(player.setPlayerName(name));
    return player;
}

void clearResidents() {
    Field* field = Field::getInstance();
    const std::vector<int64_t> ids = field->residentPlayerIds();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        Field::unsetPlayer(Player(ids[i], Status(), Position(ids[i], 0, 0, 0)));
    }
}

}

void does_not_treat_world_update_gap_as_input_ack() {
    client::ClientWorldUpdateReceiver receiver;
    client::LocalMovementPredictor& predictor = receiver.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(2, 1.0f, 0.0f, 0.0f));
    std::vector<uint8_t> baseline;
    std::vector<uint8_t> gap;
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 1, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0"};
    assert(encode(snapshot, baseline));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(baseline, applied, error));
    assert(encode(hazard(3, 3), gap));
    assert(!receiver.receive(gap, applied, error));
    assert(receiver.lastDecision() == client::WorldReceiveDecision::RequestSnapshot);
    assert(predictor.pendingCount() == 2);
    assert(std::fabs(predictor.predicted().x - 2.0f) < 0.0001f);
}

void applies_movement_ack_without_confusing_world_sequence() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(42);
    client::LocalMovementPredictor& predictor = receiver.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(2, 1.0f, 0.0f, 0.0f));
    std::vector<uint8_t> frame;
    assert(encode(movementAck(1, 1, 4, 42, 1.0f, 1), frame));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(frame, applied, error));
    assert(applied == 1);
    assert(receiver.expectedSequence() == 2);
    assert(predictor.pendingCount() == 1);
    assert(std::fabs(predictor.predicted().x - 2.0f) < 0.0001f);
}

void rebases_prediction_when_snapshot_includes_local_pose() {
    client::ClientWorldUpdateReceiver receiver;
    client::LocalMovementPredictor& predictor = receiver.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(3, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(4, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(5, 1.0f, 0.0f, 0.0f));
    std::vector<uint8_t> frame;
    assert(encode(snapshotWithLocal(10, 12, 10.0f, 3), frame));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(frame, applied, error));
    assert(applied == 1);
    assert(receiver.expectedSequence() == 11);
    assert(predictor.pendingCount() == 2);
    assert(std::fabs(predictor.predicted().x - 12.0f) < 0.0001f);
}

void ignores_foreign_session_movement_ack() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(10);
    client::LocalMovementPredictor& predictor = receiver.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    std::vector<uint8_t> frame;
    assert(encode(movementAck(1, 1, 4, 99, 8.0f, 1), frame));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(frame, applied, error));
    assert(applied == 1);
    assert(receiver.expectedSequence() == 2);
    assert(!receiver.snapshotRequested());
    assert(predictor.pendingCount() == 1);
    assert(std::fabs(predictor.predicted().x - 1.0f) < 0.0001f);
}

void reconciles_from_owner_copy_of_public_movement() {
    client::ClientWorldUpdateReceiver owner;
    owner.bindLocalSession(10);
    client::LocalMovementPredictor& predictor = owner.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    assert(predictor.predict(2, 1.0f, 0.0f, 0.0f));
    const network::WorldUpdate published = publicMovement(1, 1, 4, 10);
    server::MovementAck ack;
    ack.sessionId = 10;
    ack.x = 1.0f;
    ack.y = 0.0f;
    ack.z = 0.0f;
    ack.worldTick = 4;
    ack.lastProcessedInputSequence = 1;
    std::vector<server::MovementAck> acks;
    acks.push_back(ack);
    network::WorldUpdate ownerCopy;
    std::string copyError;
    assert(server::copyWorldUpdateForSession(published, 10, acks, ownerCopy,
                                             copyError));
    assert(ownerCopy.sequence == published.sequence);
    assert(ownerCopy.payload.find("lastProcessedInputSequence:1") !=
           std::string::npos);
    std::vector<uint8_t> frame;
    assert(encode(ownerCopy, frame));
    size_t applied = 0;
    std::string error;
    assert(owner.receive(frame, applied, error));
    assert(applied == 1);
    assert(predictor.pendingCount() == 1);
    assert(std::fabs(predictor.predicted().x - 2.0f) < 0.0001f);
}

void other_session_keeps_sequence_without_owner_ack() {
    client::ClientWorldUpdateReceiver owner;
    client::ClientWorldUpdateReceiver other;
    owner.bindLocalSession(10);
    other.bindLocalSession(20);
    client::LocalMovementPredictor& ownerPredictor = owner.localMovement();
    client::LocalMovementPredictor& otherPredictor = other.localMovement();
    ownerPredictor.reset(0.0f, 0.0f, 0.0f);
    otherPredictor.reset(0.0f, 0.0f, 0.0f);
    assert(ownerPredictor.predict(1, 1.0f, 0.0f, 0.0f));
    assert(otherPredictor.predict(1, 1.0f, 0.0f, 0.0f));

    std::vector<uint8_t> joinFrame;
    assert(encode(publicPlayerSnapshot(1, 3, 10, 7, 0.0f, "Hero"), joinFrame));
    size_t joinApplied = 0;
    std::string joinError;
    assert(owner.receive(joinFrame, joinApplied, joinError));
    assert(other.receive(joinFrame, joinApplied, joinError));
    assert(other.remotePlayers().find(7) != 0);
    assert(other.remotePlayers().find(7)->name == "Hero");
    assert(owner.remotePlayers().find(7) == 0);

    const network::WorldUpdate published = publicMovement(2, 1, 4, 10);
    server::MovementAck ack;
    ack.sessionId = 10;
    ack.x = 1.0f;
    ack.y = 0.0f;
    ack.z = 0.0f;
    ack.worldTick = 4;
    ack.lastProcessedInputSequence = 1;
    std::vector<server::MovementAck> acks;
    acks.push_back(ack);
    network::WorldUpdate ownerCopy;
    network::WorldUpdate otherCopy;
    std::string copyError;
    assert(server::copyWorldUpdateForSession(published, 10, acks, ownerCopy,
                                             copyError));
    assert(server::copyWorldUpdateForSession(published, 20, acks, otherCopy,
                                             copyError));
    assert(ownerCopy.sequence == otherCopy.sequence);
    assert(ownerCopy.eventId == otherCopy.eventId);
    assert(otherCopy.payload.find("movement=session:10") == 0);
    assert(otherCopy.payload.find("lastProcessedInputSequence") ==
           std::string::npos);
    assert(otherCopy.payload.find("movementAck=") == std::string::npos);
    assert(otherCopy.payload.find(";x:") == std::string::npos);
    assert(otherCopy.payload.find(";x=") != std::string::npos);
    assert(otherCopy.payload.find(";y=") != std::string::npos);
    assert(otherCopy.payload.find(";z=") != std::string::npos);
    assert(ownerCopy.payload.find("lastProcessedInputSequence:1") !=
           std::string::npos);

    std::vector<uint8_t> ownerFrame;
    std::vector<uint8_t> otherFrame;
    std::vector<uint8_t> hazardFrame;
    assert(encode(ownerCopy, ownerFrame));
    assert(encode(otherCopy, otherFrame));
    assert(encode(hazard(3, 2), hazardFrame));
    size_t applied = 0;
    std::string error;
    assert(owner.receive(ownerFrame, applied, error));
    assert(other.receive(otherFrame, applied, error));
    assert(owner.receive(hazardFrame, applied, error));
    assert(other.receive(hazardFrame, applied, error));
    assert(owner.expectedSequence() == 4);
    assert(other.expectedSequence() == 4);
    assert(!owner.snapshotRequested());
    assert(!other.snapshotRequested());
    assert(owner.lastDecision() == client::WorldReceiveDecision::Applied);
    assert(other.lastDecision() == client::WorldReceiveDecision::Applied);
    assert(ownerPredictor.pendingCount() == 0);
    assert(otherPredictor.pendingCount() == 1);
    assert(std::fabs(otherPredictor.predicted().x - 1.0f) < 0.0001f);
    const client::RemotePlayerPose* remote = other.remotePlayers().find(7);
    assert(remote != 0);
    assert(remote->sessionId == 10);
    assert(remote->name == "Hero");
    assert(std::fabs(remote->authority.x - 1.0f) < 0.0001f);
    assert(other.remotePlayers().find(10) == 0);
    assert(owner.remotePlayers().find(7) == 0);
}

void applies_foreign_public_pose_without_owner_ack() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(20);
    client::LocalMovementPredictor& predictor = receiver.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    std::vector<uint8_t> joinFrame;
    size_t applied = 0;
    std::string error;
    assert(encode(publicPlayerSnapshot(1, 3, 10, 7, 0.0f, "Hero"), joinFrame));
    assert(receiver.receive(joinFrame, applied, error));
    const network::WorldUpdate published = publicMovement(2, 1, 4, 10);
    server::MovementAck ack;
    ack.sessionId = 10;
    ack.x = 4.0f;
    ack.y = 0.0f;
    ack.z = 0.0f;
    ack.worldTick = 4;
    ack.lastProcessedInputSequence = 7;
    std::vector<server::MovementAck> acks;
    acks.push_back(ack);
    network::WorldUpdate otherCopy;
    std::string copyError;
    assert(server::copyWorldUpdateForSession(published, 20, acks, otherCopy,
                                             copyError));
    assert(otherCopy.payload.find("lastProcessedInputSequence") ==
           std::string::npos);
    std::vector<uint8_t> frame;
    assert(encode(otherCopy, frame));
    assert(receiver.receive(frame, applied, error));
    assert(applied == 1);
    const client::RemotePlayerPose* remote = receiver.remotePlayers().find(7);
    assert(remote != 0);
    assert(remote->sessionId == 10);
    assert(remote->name == "Hero");
    assert(std::fabs(remote->authority.x - 4.0f) < 0.0001f);
    assert(receiver.remotePlayers().find(10) == 0);
    assert(predictor.pendingCount() == 1);
    assert(std::fabs(predictor.predicted().x - 1.0f) < 0.0001f);
}

void rebases_remote_pose_from_snapshot() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(10);
    client::LocalMovementPredictor& predictor = receiver.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    const network::WorldUpdate first = publicPlayerSnapshot(1, 2, 99, 7, 1.0f, "Hero");
    const network::WorldUpdate second = publicPlayerSnapshot(2, 8, 99, 7, 10.0f, "Hero");
    std::vector<uint8_t> firstFrame;
    std::vector<uint8_t> secondFrame;
    assert(encode(first, firstFrame));
    assert(encode(second, secondFrame));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(firstFrame, applied, error));
    assert(receiver.remotePlayers().find(7) != 0);
    assert(receiver.remotePlayers().find(99) == 0);
    assert(std::fabs(receiver.remotePlayers().find(7)->authority.x - 1.0f) <
           0.0001f);
    assert(receiver.receive(secondFrame, applied, error));
    const client::RemotePlayerPose* remote = receiver.remotePlayers().find(7);
    assert(remote != 0);
    assert(std::fabs(remote->authority.x - 10.0f) < 0.0001f);
    assert(std::fabs(remote->rendered.x - 10.0f) < 0.0001f);
    assert(receiver.remotePlayers().find(10) == 0);
    assert(predictor.pendingCount() == 1);
    assert(std::fabs(predictor.predicted().x - 1.0f) < 0.0001f);
    assert(second.payload.find("lastProcessedInputSequence") ==
           std::string::npos);
    assert(second.payload.find("local.") == std::string::npos);
}

void reconnect_snapshot_keeps_one_remote_for_gameplay_id() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(10);
    std::vector<uint8_t> firstFrame;
    std::vector<uint8_t> secondFrame;
    assert(encode(publicPlayerSnapshot(1, 2, 99, 7, 1.0f, "Hero"), firstFrame));
    assert(encode(publicPlayerSnapshot(2, 8, 100, 7, 6.0f, "Hero"), secondFrame));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(firstFrame, applied, error));
    assert(receiver.remotePlayers().count() == 1);
    assert(receiver.remotePlayers().find(7)->sessionId == 99);
    assert(receiver.receive(secondFrame, applied, error));
    assert(receiver.remotePlayers().count() == 1);
    const client::RemotePlayerPose* remote = receiver.remotePlayers().find(7);
    assert(remote != 0);
    assert(remote->sessionId == 100);
    assert(remote->name == "Hero");
    assert(std::fabs(remote->authority.x - 6.0f) < 0.0001f);
    assert(receiver.remotePlayers().find(99) == 0);
    assert(receiver.remotePlayers().find(100) == 0);
}

void idle_remote_appears_from_join_snapshot() {
    clearResidents();
    Field* field = Field::getInstance();
    const int64_t idleId = 15222;
    const int64_t observerId = 15223;
    assert(field->setPlayer(namedResident(idleId, 6.0f, "Idle")));
    assert(field->setPlayer(namedResident(observerId, 0.0f, "Observer")));
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    std::vector<server::MovementAck> publishAcks;
    assert(applier.capturePublicSnapshotIfNewSessions(0, 4, updates, publishAcks,
                                                      error));
    assert(updates.empty());
    client::ClientWorldUpdateReceiver idleReceiver;
    idleReceiver.bindLocalSession(observerId);
    assert(idleReceiver.remotePlayers().find(idleId) == 0);
    assert(applier.capturePublicSnapshotIfNewSessions(1, 4, updates, publishAcks,
                                                      error));
    network::WorldUpdate observerCopy;
    assert(server::copyWorldUpdateForSession(updates[0], observerId, publishAcks,
                                             observerCopy, error));
    std::vector<uint8_t> joinFrame;
    assert(encode(observerCopy, joinFrame));
    size_t applied = 0;
    assert(idleReceiver.receive(joinFrame, applied, error));
    const client::RemotePlayerPose* joined = idleReceiver.remotePlayers().find(idleId);
    assert(joined != 0);
    assert(std::fabs(joined->authority.x - 6.0f) < 0.0001f);
}

void idle_remote_appears_from_captured_snapshot() {
    clearResidents();
    Field* field = Field::getInstance();
    const int64_t idleId = 15220;
    const int64_t observerId = 15221;
    assert(field->setPlayer(namedResident(idleId, 9.0f, "Idle")));
    assert(field->setPlayer(namedResident(observerId, 0.0f, "Observer")));
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.capturePublicSnapshot(3, updates, error));
    network::WorldUpdate observerCopy;
    assert(server::copyWorldUpdateForSession(updates[0], observerId,
                                             applier.snapshotLocalAcks(),
                                             observerCopy, error));
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(observerId);
    client::LocalMovementPredictor& predictor = receiver.localMovement();
    predictor.reset(0.0f, 0.0f, 0.0f);
    assert(predictor.predict(1, 1.0f, 0.0f, 0.0f));
    std::vector<uint8_t> frame;
    assert(encode(observerCopy, frame));
    size_t applied = 0;
    assert(receiver.receive(frame, applied, error));
    assert(applied == 1);
    const client::RemotePlayerPose* remote = receiver.remotePlayers().find(idleId);
    assert(remote != 0);
    assert(std::fabs(remote->authority.x - 9.0f) < 0.0001f);
    assert(receiver.remotePlayers().find(observerId) == 0);
    assert(predictor.pendingCount() == 1);
    assert(std::fabs(predictor.predicted().x - 1.0f) < 0.0001f);
}

void does_not_invent_remote_pose_from_delta_only() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(20);
    std::vector<uint8_t> frame;
    assert(encode(publicMovement(1, 1, 4, 99), frame));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(frame, applied, error));
    assert(applied == 1);
    assert(receiver.remotePlayers().count() == 0);
    assert(receiver.remotePlayers().find(99) == 0);
}

} // namespace client_prediction_sync_tests

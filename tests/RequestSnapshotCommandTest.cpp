#include <cassert>
#include <string>
#include <vector>

#include "ClientWorldUpdateReceiver.h"
#include "Field.h"
#include "NetworkCommand.h"
#include "NetworkFrameCodec.h"
#include "Player.h"
#include "Position.h"
#include "ServerCommandDispatcher.h"
#include "SessionRegistry.h"
#include "Status.h"
#include "WorldFrameApplier.h"
#include "WorldInputQueue.h"
#include "WorldUpdateFrameCodec.h"

namespace request_snapshot_command_tests {
namespace {

void clearFieldPlayers() {
    Field* field = Field::getInstance();
    const std::vector<PlayerPoseSnapshot> poses = field->publicPlayerPoses();
    for (std::size_t i = 0; i < poses.size(); ++i) {
        const int64_t sessionId = poses[i].sessionId;
        Field::unsetPlayer(Player(sessionId, Status(), Position(sessionId, 0, 0, 0)));
    }
}

Player namedResident(int64_t playerId, float x, const char* name) {
    Player player(playerId, Status(), Position(playerId, x, 0.0f, 0.0f));
    assert(player.setPlayerName(name));
    return player;
}

bool encodeUpdate(const network::WorldUpdate& update, std::vector<uint8_t>& bytes) {
    std::string error;
    return network::encodeWorldUpdateFrame(update, bytes, error);
}

network::WorldUpdate hazard(uint64_t sequence, uint64_t eventId) {
    return {1, network::UpdateKind::Event, 8, sequence, eventId,
            "etherHazard=severity:2;instability:3"};
}

}

void validates_empty_payload_request_snapshot_on_protocol_v1() {
    const network::NetworkCommand command =
        network::makeRequestSnapshotCommand(21);
    std::string error;
    assert(command.version == network::CURRENT_PROTOCOL_VERSION);
    assert(command.version == 1);
    assert(command.type == network::CommandType::RequestSnapshot);
    assert(command.sessionId == 21);
    assert(command.payload.empty());
    assert(network::validateCommand(command, error));
    assert(error.empty());

    std::vector<uint8_t> frame;
    network::NetworkCommand decoded = {};
    assert(network::encodeFrame(command, frame, error));
    assert(network::decodeFrame(frame, decoded, error));
    assert(decoded.type == network::CommandType::RequestSnapshot);
    assert(decoded.payload.empty());
    assert(decoded.sessionId == 21);
    assert(decoded.version == 1);
}

void client_builds_request_after_sequence_gap_or_reconnect() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(21);
    std::vector<uint8_t> baseline;
    std::vector<uint8_t> gap;
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 1, 1, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0"};
    assert(encodeUpdate(snapshot, baseline));
    size_t applied = 0;
    std::string error;
    assert(receiver.receive(baseline, applied, error));
    assert(encodeUpdate(hazard(5, 5), gap));
    assert(!receiver.receive(gap, applied, error));
    assert(receiver.snapshotRequested());
    assert(receiver.lastDecision() == client::WorldReceiveDecision::RequestSnapshot);

    network::NetworkCommand command = {};
    assert(receiver.tryBuildRequestSnapshotCommand(command));
    assert(command.type == network::CommandType::RequestSnapshot);
    assert(command.payload.empty());
    assert(command.sessionId == 21);
    assert(command.version == 1);

    client::ClientWorldUpdateReceiver reconnecting;
    reconnecting.beginReconnect();
    assert(reconnecting.snapshotRequested());
    assert(!reconnecting.tryBuildRequestSnapshotCommand(command));
    reconnecting.bindLocalSession(21);
    assert(reconnecting.tryBuildRequestSnapshotCommand(command));
    assert(command.sessionId == 21);
}

void dispatcher_accepts_logged_in_request_and_coalesces_one_snapshot() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    assert(field->setPlayer(namedResident(21, 0.0f, "Alpha")));
    assert(field->setPlayer(namedResident(22, 4.0f, "Beta")));

    session::SessionRegistry registry;
    server::WorldInputQueue queue;
    server::ServerCommandDispatcher dispatcher(registry, queue);
    dispatcher.beginFrame(3);
    const session::SessionInfo a = registry.login("snap-a");
    const session::SessionInfo b = registry.login("snap-b");
    const network::NetworkCommand first =
        network::makeRequestSnapshotCommand(a.internalId);
    const network::NetworkCommand second =
        network::makeRequestSnapshotCommand(b.internalId);
    const network::NetworkCommand duplicate =
        network::makeRequestSnapshotCommand(a.internalId);
    const server::CommandDispatchResult acceptedA = dispatcher.dispatch(first);
    const server::CommandDispatchResult acceptedB = dispatcher.dispatch(second);
    const server::CommandDispatchResult limited = dispatcher.dispatch(duplicate);
    assert(acceptedA.accepted);
    assert(acceptedB.accepted);
    assert(!limited.accepted);
    assert(dispatcher.snapshotRequestCount() == 2);
    assert(queue.pendingCount() == 0);

    const network::NetworkCommand anonymous =
        network::makeRequestSnapshotCommand(9999);
    const server::CommandDispatchResult rejected = dispatcher.dispatch(anonymous);
    assert(!rejected.accepted);
    assert(dispatcher.snapshotRequestCount() == 2);

    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    std::vector<server::MovementAck> publishAcks;
    assert(applier.capturePublicSnapshotIfNewSessions(
        dispatcher.snapshotRequestCount(), 3, updates, publishAcks, error));
    assert(updates.size() == 1);
    assert(updates[0].kind == network::UpdateKind::Snapshot);
    assert(updates[0].payload.find("player.") != std::string::npos);
    assert(updates[0].payload.find("session=21") != std::string::npos);
    assert(updates[0].payload.find("session=22") != std::string::npos);
    assert(updates[0].payload.find("movement=") == std::string::npos);
    network::WorldUpdate ownerCopy;
    assert(server::copyWorldUpdateForSession(updates[0], 21,
                                             applier.snapshotLocalAcks(),
                                             ownerCopy, error));
    assert(ownerCopy.sequence == updates[0].sequence);
    assert(ownerCopy.payload.find("local.x=") != std::string::npos);

    assert(Field::unsetPlayer(Player(21, Status(), Position(21, 0, 0, 0))));
    assert(Field::unsetPlayer(Player(22, Status(), Position(22, 0, 0, 0))));
}

void snapshot_after_request_resumes_events_without_treating_movement_as_snapshot() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.bindLocalSession(21);
    receiver.beginReconnect();
    assert(receiver.snapshotRequested());

    const network::WorldUpdate movement = {
        1, network::UpdateKind::Event, 4, 1, 1,
        "movement=session:21;dx=1;dy=0;dz=0;clientInputSequence=1;inputSequence=1"};
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 4, 2, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "player.count=1;player.0.session=21;player.0.x=0;player.0.y=0;player.0.z=0;"
        "player.0.name=Hero"};
    std::vector<uint8_t> mixed;
    assert(encodeUpdate(movement, mixed));
    std::vector<uint8_t> snapshotBytes;
    assert(encodeUpdate(snapshot, snapshotBytes));
    mixed.insert(mixed.end(), snapshotBytes.begin(), snapshotBytes.end());

    size_t applied = 0;
    std::string error;
    assert(receiver.receive(mixed, applied, error));
    assert(applied == 1);
    assert(!receiver.snapshotRequested());
    assert(receiver.expectedSequence() == 3);
    assert(receiver.environment().value().players.size() == 1);

    std::vector<uint8_t> nextEvent;
    assert(encodeUpdate(hazard(3, 3), nextEvent));
    assert(receiver.receive(nextEvent, applied, error));
    assert(applied == 1);
    assert(receiver.expectedSequence() == 4);
    assert(receiver.hazardEffects().size() == 1);
}

} // namespace request_snapshot_command_tests

#include <cassert>
#include <cmath>
#include <string>
#include <vector>

#include "Field.h"
#include "FieldSessionPresence.h"
#include "WorldFrameApplier.h"
#include "WorldInputTick.h"
#include "WorldUpdate.h"

namespace field_session_presence_tests {

void clearFieldPlayers() {
    Field* field = Field::getInstance();
    const std::vector<PlayerPoseSnapshot> poses = field->publicPlayerPoses();
    for (std::size_t i = 0; i < poses.size(); ++i) {
        const int64_t sessionId = poses[i].sessionId;
        Field::unsetPlayer(Player(sessionId, Status(), Position(sessionId, 0, 0, 0)));
    }
}

void join_snapshot_includes_logging_in_session_at_temporary_origin() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    const int64_t sessionId = 21;
    assert(server::FieldSessionPresence::placeAfterLogin(sessionId));
    const Player* placed = field->findPlayer(sessionId);
    assert(placed != 0);
    assert(std::fabs(placed->getPosition().getX()) < 0.0001f);
    assert(std::fabs(placed->getPosition().getY()) < 0.0001f);
    assert(std::fabs(placed->getPosition().getZ()) < 0.0001f);

    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    std::vector<server::MovementAck> publishAcks;
    assert(applier.capturePublicSnapshotIfNewSessions(1, 4, updates, publishAcks,
                                                      error));
    assert(updates.size() == 1);
    assert(updates[0].kind == network::UpdateKind::Snapshot);
    assert(updates[0].payload.find("player.count=1") != std::string::npos);
    assert(updates[0].payload.find("session=21") != std::string::npos);
    assert(updates[0].payload.find("player.0.x=0") != std::string::npos);
    assert(updates[0].payload.find("player.0.y=0") != std::string::npos);
    assert(updates[0].payload.find("player.0.z=0") != std::string::npos);
    assert(Field::unsetPlayer(
        Player(sessionId, Status(), Position(sessionId, 0, 0, 0))));
}

void join_snapshot_includes_idle_others_already_on_the_field() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    const Status status;
    const int64_t idleId = 99;
    const int64_t joinerId = 21;
    assert(field->setPlayer(Player(idleId, status, Position(idleId, 6.0f, 0.0f, 0.0f))));
    assert(server::FieldSessionPresence::placeAfterLogin(joinerId));

    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    std::vector<server::MovementAck> publishAcks;
    assert(applier.capturePublicSnapshotIfNewSessions(1, 5, updates, publishAcks,
                                                      error));
    assert(updates.size() == 1);
    assert(updates[0].kind == network::UpdateKind::Snapshot);
    assert(updates[0].payload.find("player.count=2") != std::string::npos);
    assert(updates[0].payload.find("session=99") != std::string::npos);
    assert(updates[0].payload.find("session=21") != std::string::npos);
    assert(updates[0].payload.find("6") != std::string::npos);

    assert(Field::unsetPlayer(Player(idleId, status, Position(idleId, 0, 0, 0))));
    assert(Field::unsetPlayer(Player(joinerId, status, Position(joinerId, 0, 0, 0))));
}

void login_placement_does_not_emit_a_movement_event() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    const int64_t sessionId = 21;
    assert(server::FieldSessionPresence::placeAfterLogin(sessionId));
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    const server::WorldFrameInputs frame = {6, std::vector<server::WorldInput>()};
    assert(applier.apply(frame, updates, error));
    for (std::size_t i = 0; i < updates.size(); ++i) {
        assert(updates[i].payload.find("movement=") == std::string::npos);
        assert(updates[i].kind != network::UpdateKind::Snapshot);
    }
    std::vector<server::MovementAck> publishAcks;
    assert(applier.capturePublicSnapshotIfNewSessions(0, 6, updates, publishAcks,
                                                      error));
    for (std::size_t i = 0; i < updates.size(); ++i) {
        assert(updates[i].kind != network::UpdateKind::Snapshot);
    }
    assert(Field::unsetPlayer(
        Player(sessionId, Status(), Position(sessionId, 0, 0, 0))));
}

void logout_removes_the_session_from_the_field() {
    clearFieldPlayers();
    const int64_t sessionId = 21;
    assert(server::FieldSessionPresence::placeAfterLogin(sessionId));
    assert(Field::getInstance()->hasPlayer(sessionId));
    assert(server::FieldSessionPresence::removeAfterLogout(sessionId));
    assert(!Field::getInstance()->hasPlayer(sessionId));
}

} // namespace field_session_presence_tests

#include <cassert>
#include <cmath>
#include <string>
#include <vector>

#include "AuthenticatedPlayerIdPort.h"
#include "Field.h"
#include "FieldSessionPresence.h"
#include "LoginFieldSpawnSettings.h"
#include "WorldFrameApplier.h"
#include "WorldInputTick.h"
#include "WorldUpdate.h"

namespace field_session_presence_tests {
namespace {

class FixedPlayerIdPort : public server::AuthenticatedPlayerIdPort {
public:
    explicit FixedPlayerIdPort(int64_t playerId) : playerId(playerId) {}
    bool resolvePlayerId(int64_t, const std::string&, int64_t& out) const {
        out = playerId;
        return playerId > 0;
    }

private:
    int64_t playerId;
};

class ClaimedIdPlayerIdPort : public server::AuthenticatedPlayerIdPort {
public:
    bool resolvePlayerId(int64_t, const std::string& claimedId,
                         int64_t& out) const {
        if (claimedId == "user-a") {
            out = 9001;
            return true;
        }
        if (claimedId == "user-b") {
            out = 9002;
            return true;
        }
        return false;
    }
};

void clearFieldPlayers() {
    Field* field = Field::getInstance();
    const std::vector<int64_t> ids = field->residentPlayerIds();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        Field::unsetPlayer(Player(ids[i], Status(), Position(ids[i], 0, 0, 0)));
    }
    server::FieldSessionPresence::usePlayerIdPort(0);
    server::FieldSessionPresence::useSpawnSettings(
        server::LoginFieldSpawnSettings());
}

}

void join_snapshot_includes_logging_in_session_at_temporary_origin() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    const int64_t sessionId = 21;
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    assert(server::FieldSessionPresence::placeAfterLogin(sessionId, "alice"));
    const Player* placed = field->findPlayer(sessionId);
    assert(placed != 0);
    assert(placed->getPlayerId() != sessionId);
    assert(placed->getPlayerId() != 9001);
    assert(placed->getAuthPlayerId() == 9001);
    assert(placed->getPlayerName() == "Hero");
    assert(placed->getPlayerName() != "alice");
    assert(placed->getStatus().getHp() == 10);
    assert(placed->getStatus().getMp() == 10);
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
    assert(updates[0].payload.find("player.0.id=") != std::string::npos);
    assert(updates[0].payload.find("player.0.name=Hero") != std::string::npos);
    assert(updates[0].payload.find("name=alice") == std::string::npos);
    assert(updates[0].payload.find("session=9001") == std::string::npos);
    assert(updates[0].payload.find("id=9001") == std::string::npos);
    assert(Field::unsetPlayer(
        Player(sessionId, Status(), Position(sessionId, 0, 0, 0))));
    clearFieldPlayers();
}

void join_snapshot_includes_idle_others_already_on_the_field() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    const Status status;
    const int64_t idleId = 99;
    const int64_t joinerId = 21;
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    Player idle(idleId, status, Position(idleId, 6.0f, 0.0f, 0.0f));
    assert(idle.setPlayerName("Watcher"));
    assert(field->setPlayer(idle));
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    assert(server::FieldSessionPresence::placeAfterLogin(joinerId, "alice"));

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
    clearFieldPlayers();
}

void login_placement_does_not_emit_a_movement_event() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    const int64_t sessionId = 21;
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
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
    clearFieldPlayers();
}

void logout_removes_the_session_from_the_field() {
    clearFieldPlayers();
    const int64_t sessionId = 21;
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    assert(server::FieldSessionPresence::placeAfterLogin(sessionId, "alice"));
    Field* field = Field::getInstance();
    assert(field->hasPlayer(sessionId));
    const int64_t gameplayId = field->findPlayer(sessionId)->getPlayerId();
    assert(server::FieldSessionPresence::removeAfterLogout(sessionId));
    assert(!field->hasPlayer(sessionId));
    assert(field->hasPlayer(gameplayId));
    assert(field->publicPlayerPoses().empty());
    clearFieldPlayers();
}

void login_placement_uses_configured_pose_and_status() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    const server::LoginFieldSpawnSettings settings(1.0f, 2.0f, 3.0f, 20, 30,
                                                   1024, 1024);
    assert(server::FieldSessionPresence::placeAfterLogin(21, "alice", settings));
    const Player* placed = Field::getInstance()->findPlayer(21);
    assert(placed != 0);
    assert(std::fabs(placed->getPosition().getX() - 1.0f) < 0.0001f);
    assert(std::fabs(placed->getPosition().getY() - 2.0f) < 0.0001f);
    assert(std::fabs(placed->getPosition().getZ() - 3.0f) < 0.0001f);
    assert(placed->getStatus().getHp() == 20);
    assert(placed->getStatus().getMp() == 30);
    clearFieldPlayers();
}

void login_placement_clamps_hp_mp_to_spawn_max() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    const server::LoginFieldSpawnSettings settings(0.0f, 0.0f, 0.0f, 4096, 2048,
                                                   1024, 1024);
    assert(server::FieldSessionPresence::placeAfterLogin(21, "alice", settings));
    const Player* placed = Field::getInstance()->findPlayer(21);
    assert(placed != 0);
    assert(placed->getStatus().getHp() == 1024);
    assert(placed->getStatus().getMp() == 1024);
    clearFieldPlayers();
}

void reconnect_rebinds_new_session_to_the_same_entity() {
    clearFieldPlayers();
    Field* field = Field::getInstance();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    const server::LoginFieldSpawnSettings settings(4.0f, 0.0f, 0.0f, 15, 10,
                                                   1024, 1024);
    assert(server::FieldSessionPresence::placeAfterLogin(21, "alice", settings));
    const int64_t gameplayId = field->findPlayer(21)->getPlayerId();
    assert(server::FieldSessionPresence::removeAfterLogout(21));
    assert(server::FieldSessionPresence::placeAfterLogin(22, "not-a-display-name"));
    const Player* rebound = field->findPlayer(22);
    assert(rebound != 0);
    assert(rebound->getPlayerId() == gameplayId);
    assert(rebound->getAuthPlayerId() == 9001);
    assert(rebound->getPlayerName() == "Hero");
    Player* mutableRebound = field->findPlayer(22);
    mutableRebound->setAuthPlayerId(1);
    assert(mutableRebound->getAuthPlayerId() == 9001);
    assert(std::fabs(rebound->getPosition().getX() - 4.0f) < 0.0001f);
    assert(rebound->getStatus().getHp() == 15);
    assert(!field->hasPlayer(21));
    const std::vector<PlayerPoseSnapshot> poses = field->publicPlayerPoses();
    assert(poses.size() == 1);
    assert(poses[0].sessionId == 22);
    assert(poses[0].gameplayId == gameplayId);
    clearFieldPlayers();
}

void empty_player_name_placement_is_rejected() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9001, ""));
    assert(!server::FieldSessionPresence::placeAfterLogin(21, "alice"));
    assert(!Field::getInstance()->hasPlayer(21));
    server::LoginFieldSpawnSettings settings;
    settings.playerName = "";
    assert(!server::FieldSessionPresence::placeAfterLogin(21, "alice", settings));
    assert(!Field::getInstance()->hasPlayer(21));
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    assert(server::FieldSessionPresence::placeAfterLogin(21, "alice"));
    assert(Field::getInstance()->findPlayer(21)->getPlayerName() == "Hero");
    clearFieldPlayers();
}

void whitespace_only_player_name_placement_is_rejected() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9001, "   "));
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9001, "\t\n"));
    server::LoginFieldSpawnSettings settings;
    settings.playerName = "   ";
    assert(!server::FieldSessionPresence::placeAfterLogin(21, "alice", settings));
    assert(!Field::getInstance()->hasPlayer(21));
    clearFieldPlayers();
}

void claimed_player_name_survives_logout_and_field_unset() {
    clearFieldPlayers();
    const ClaimedIdPlayerIdPort port;
    server::FieldSessionPresence::usePlayerIdPort(&port);
    server::LoginFieldSpawnSettings settings;
    settings.playerName = "Hero";
    assert(server::FieldSessionPresence::placeAfterLogin(21, "user-a", settings));
    Field* field = Field::getInstance();
    const int64_t gameplayId = field->findPlayer(21)->getPlayerId();
    assert(server::FieldSessionPresence::removeAfterLogout(21));
    assert(Field::unsetPlayer(
        Player(gameplayId, Status(), Position(gameplayId, 0, 0, 0))));
    assert(!field->hasPlayer(gameplayId));
    assert(!server::FieldSessionPresence::placeAfterLogin(22, "user-b", settings));
    assert(!field->hasPlayer(22));
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9002, "Hero"));
    clearFieldPlayers();
}

void trimmed_player_name_collides_with_existing_claim() {
    clearFieldPlayers();
    const ClaimedIdPlayerIdPort port;
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "  Hero  "));
    assert(server::FieldSessionPresence::placeAfterLogin(21, "user-a"));
    assert(Field::getInstance()->findPlayer(21)->getPlayerName() == "Hero");
    server::LoginFieldSpawnSettings settings;
    settings.playerName = " Hero";
    assert(!server::FieldSessionPresence::placeAfterLogin(22, "user-b", settings));
    assert(!Field::getInstance()->hasPlayer(22));
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9002, "\tHero"));
    assert(Field::getInstance()->findPlayer(21)->getPlayerName() == "Hero");
    clearFieldPlayers();
}

void player_cannot_rename_operator_can() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    assert(server::FieldSessionPresence::placeAfterLogin(21, "alice"));
    assert(!server::FieldSessionPresence::playerSetPlayerName(21, "Villain"));
    assert(Field::getInstance()->findPlayer(21)->getPlayerName() == "Hero");
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Mage"));
    assert(Field::getInstance()->findPlayer(21)->getPlayerName() == "Mage");
    clearFieldPlayers();
}

void duplicate_player_name_is_rejected() {
    clearFieldPlayers();
    const ClaimedIdPlayerIdPort port;
    server::FieldSessionPresence::usePlayerIdPort(&port);
    server::LoginFieldSpawnSettings settings;
    settings.playerName = "Hero";
    assert(server::FieldSessionPresence::placeAfterLogin(21, "user-a", settings));
    assert(!server::FieldSessionPresence::placeAfterLogin(22, "user-b", settings));
    assert(Field::getInstance()->hasPlayer(21));
    assert(!Field::getInstance()->hasPlayer(22));
    clearFieldPlayers();
}

} // namespace field_session_presence_tests

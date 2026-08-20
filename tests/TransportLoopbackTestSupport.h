#ifndef SEED_TRANSPORT_LOOPBACK_TEST_SUPPORT_H
#define SEED_TRANSPORT_LOOPBACK_TEST_SUPPORT_H

#include <cassert>
#include <string>
#include <vector>

#include "Field.h"
#include "FieldSessionPresence.h"
#include "LoginFieldSpawnSettings.h"
#include "Player.h"
#include "Position.h"
#include "ServerCommandDispatcher.h"
#include "ServerRuntime.h"
#include "Status.h"
#include "WorldFrameApplier.h"

namespace seed_test {

inline void clearFieldPlayers() {
    Field* field = Field::getInstance();
    const std::vector<PlayerPoseSnapshot> poses = field->publicPlayerPoses();
    for (std::size_t i = 0; i < poses.size(); ++i) {
        const int64_t sessionId = poses[i].sessionId;
        Field::unsetPlayer(Player(sessionId, Status(), Position(sessionId, 0, 0, 0)));
    }
}

inline void resetFieldSessionPresence() {
    server::FieldSessionPresence::useSpawnSettings(server::LoginFieldSpawnSettings());
    server::FieldSessionPresence::usePlayerIdPort(0);
}

inline void serverTick(server::ServerRuntime& runtime,
                       server::ServerCommandDispatcher& dispatcher,
                       server::WorldFrameApplier& applier,
                       server::ServerFrameResult& frame) {
    std::string error;
    frame = runtime.processFrame(dispatcher, error);
    assert(error.empty());
    std::vector<network::WorldUpdate> updates;
    assert(applier.apply(
        server::WorldFrameInputs{frame.worldTick, frame.inputs}, updates, error));
    std::vector<server::MovementAck> acks = applier.ownerMovementAcks();
    assert(applier.capturePublicSnapshotIfNewSessions(
        frame.newAuthenticatedSessions + frame.snapshotRequests, frame.worldTick,
        updates, acks, error));
    if (updates.empty()) return;
    std::string publishError;
    runtime.publishWorldUpdates(updates, acks, publishError);
}

}

#endif

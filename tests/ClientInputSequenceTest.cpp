#include <cassert>
#include <cmath>
#include <string>
#include <vector>

#include "Field.h"
#include "MovementCommandHandler.h"
#include "WorldFrameApplier.h"
#include "WorldInputTick.h"

namespace client_input_sequence_tests {
namespace {

void placePlayer(int64_t playerId) {
    Field* field = Field::getInstance();
    const Status status;
    const Position position(playerId, 0.0f, 0.0f, 0.0f);
    assert(field->setPlayer(Player(playerId, status, position)));
}

network::NetworkCommand moveCommand(int64_t playerId, const std::string& payload) {
    return {network::CURRENT_PROTOCOL_VERSION, network::CommandType::Move,
            playerId, payload};
}

bool hasOwnerAck(const server::WorldFrameApplier& applier, int64_t sessionId,
                 uint64_t lastProcessed) {
    const std::vector<server::MovementAck>& acks = applier.ownerMovementAcks();
    for (size_t i = 0; i < acks.size(); ++i) {
        if (acks[i].sessionId == sessionId &&
            acks[i].lastProcessedInputSequence == lastProcessed) {
            return true;
        }
    }
    return false;
}

bool ackReportsPose(const server::WorldFrameApplier& applier, float x, float y,
                    float z) {
    const std::vector<server::MovementAck>& acks = applier.ownerMovementAcks();
    for (size_t i = 0; i < acks.size(); ++i) {
        if (std::fabs(acks[i].x - x) < 0.0001f &&
            std::fabs(acks[i].y - y) < 0.0001f &&
            std::fabs(acks[i].z - z) < 0.0001f && acks[i].worldTick > 0) {
            return true;
        }
    }
    return false;
}

bool publicUpdatesHideOwnerAck(const std::vector<network::WorldUpdate>& updates) {
    for (size_t i = 0; i < updates.size(); ++i) {
        if (updates[i].payload.find("movementAck=") == 0) return false;
        if (updates[i].payload.find("lastProcessedInputSequence") !=
            std::string::npos) {
            return false;
        }
    }
    return true;
}

bool hasPublicMovement(const std::vector<network::WorldUpdate>& updates,
                       int64_t sessionId) {
    const std::string needle =
        "movement=session:" + std::to_string(sessionId);
    for (size_t i = 0; i < updates.size(); ++i) {
        if (updates[i].payload.find(needle) == 0) return true;
    }
    return false;
}

}

void acks_monotonic_client_input_sequences() {
    const int64_t playerId = 15001;
    placePlayer(playerId);
    server::WorldInputQueue queue;
    server::MovementCommandHandler handler(queue);
    assert(handler.handle(moveCommand(playerId, "1:1,0,0")).accepted);
    assert(handler.handle(moveCommand(playerId, "2:1,0,0")).accepted);
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    Field* field = Field::getInstance();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    const Player* player = field->findPlayer(playerId);
    assert(player != nullptr);
    assert(std::fabs(player->getPosition().getX() - 2.0f) < 0.0001f);
    size_t movementEvents = 0;
    for (size_t i = 0; i < updates.size(); ++i) {
        if (updates[i].payload.find("movement=session:") == 0) ++movementEvents;
        if (i > 0) {
            assert(updates[i].sequence == updates[i - 1].sequence + 1);
        }
    }
    assert(movementEvents == 2);
    assert(hasPublicMovement(updates, playerId));
    assert(publicUpdatesHideOwnerAck(updates));
    assert(hasOwnerAck(applier, playerId, 2));
    assert(ackReportsPose(applier, 2.0f, 0.0f, 0.0f));
}

void ignores_duplicate_client_input_sequence() {
    const int64_t playerId = 15002;
    placePlayer(playerId);
    server::WorldInputQueue queue;
    server::MovementCommandHandler handler(queue);
    assert(handler.handle(moveCommand(playerId, "1:1,0,0")).accepted);
    const server::MovementResult duplicate = handler.handle(moveCommand(playerId, "1:1,0,0"));
    assert(!duplicate.accepted);
    assert(duplicate.ignored);
    assert(queue.pendingCount() == 1);
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    Field* field = Field::getInstance();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    const Player* player = field->findPlayer(playerId);
    assert(player != nullptr);
    assert(std::fabs(player->getPosition().getX() - 1.0f) < 0.0001f);
    size_t movementEvents = 0;
    for (size_t i = 0; i < updates.size(); ++i) {
        if (updates[i].payload.find("movement=session:") == 0) ++movementEvents;
        if (i > 0) {
            assert(updates[i].sequence == updates[i - 1].sequence + 1);
        }
    }
    assert(movementEvents == 1);
    assert(hasPublicMovement(updates, playerId));
    assert(publicUpdatesHideOwnerAck(updates));
    assert(hasOwnerAck(applier, playerId, 1));
}

void ignores_out_of_order_or_stale_client_input_sequence() {
    const int64_t playerId = 15003;
    placePlayer(playerId);
    server::WorldInputQueue queue;
    server::MovementCommandHandler handler(queue);
    assert(handler.handle(moveCommand(playerId, "2:1,0,0")).accepted);
    const server::MovementResult stale = handler.handle(moveCommand(playerId, "1:1,0,0"));
    assert(!stale.accepted);
    assert(stale.ignored);
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    Field* field = Field::getInstance();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    const Player* player = field->findPlayer(playerId);
    assert(player != nullptr);
    assert(std::fabs(player->getPosition().getX() - 1.0f) < 0.0001f);
    assert(std::fabs(player->getPosition().getY()) < 0.0001f);
}

void rejects_client_input_sequence_too_far_ahead() {
    const int64_t playerId = 15004;
    placePlayer(playerId);
    server::WorldInputQueue queue;
    server::MovementCommandHandler handler(queue);
    const server::MovementResult far =
        handler.handle(moveCommand(playerId, "65:1,0,0"));
    assert(!far.accepted);
    assert(!far.ignored);
    assert(queue.pendingCount() == 0);
}

void keeps_legacy_unsequenced_move_payload() {
    const int64_t playerId = 15005;
    placePlayer(playerId);
    server::WorldInputQueue queue;
    server::MovementCommandHandler handler(queue);
    assert(handler.handle(moveCommand(playerId, "1,0,0")).accepted);
    assert(handler.handle(moveCommand(playerId, "1,0,0")).accepted);
    assert(queue.pendingCount() == 2);
}

} // namespace client_input_sequence_tests

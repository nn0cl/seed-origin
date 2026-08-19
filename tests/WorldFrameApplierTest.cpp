#include <cassert>

#include "WorldFrameApplier.h"

namespace world_frame_applier_tests {

void applies_valid_actions_and_returns_events() {
    Field* field = Field::getInstance();
    server::WorldFrameApplier applier(*field);
    ActionQueue queue;
    ServerTick tick(queue);
    const Status status;
    Player player;
    assert(tick.submit(Action(0, nullptr, nullptr, status)));
    const FrameActions frame = tick.advanceFrame();
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    assert(updates.size() == 1);
    assert(updates[0].worldTick == frame.worldTick);
}

void rejects_invalid_action_before_field_mutation() {
    Field* field = Field::getInstance();
    server::WorldFrameApplier applier(*field);
    ActionQueue queue;
    ServerTick tick(queue);
    const Status status;
    Player player;
    const Action invalid(99, &player, nullptr, status);
    FrameActions frame = {1, std::vector<QueuedAction>()};
    frame.actions.push_back(QueuedAction(1, invalid));
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(!applier.apply(frame, updates, error));
    assert(updates.empty());
}

void applies_movement_intents_after_target_validation() {
    Field* field = Field::getInstance();
    const Status status;
    const int64_t playerId = 9001;
    const Position position(playerId, 0.0f, 0.0f, 0.0f);
    assert(field->setPlayer(Player(playerId, status, position)));
    server::MovementIntentQueue queue;
    assert(queue.enqueue(playerId, 1.0f, 2.0f, 3.0f));
    server::WorldFrameApplier applier(*field, queue);
    ActionQueue actions;
    ServerTick tick(actions);
    const FrameActions frame = tick.advanceFrame();
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    assert(queue.pendingCount() == 0);
}

void restores_movement_intents_when_target_is_missing() {
    Field* field = Field::getInstance();
    server::MovementIntentQueue queue;
    assert(queue.enqueue(9002, 1.0f, 2.0f, 3.0f));
    server::WorldFrameApplier applier(*field, queue);
    ActionQueue actions;
    ServerTick tick(actions);
    const FrameActions frame = tick.advanceFrame();
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(!applier.apply(frame, updates, error));
    assert(queue.pendingCount() == 1);
}

void applies_combat_damage_after_range_validation() {
    Field* field = Field::getInstance();
    const Position attackerPosition(9101, 0.0f, 0.0f, 0.0f);
    const Position targetPosition(9102, 3.0f, 0.0f, 0.0f);
    assert(field->setPlayer(Player(9101, Status(100, 10), attackerPosition)));
    assert(field->setPlayer(Player(9102, Status(100, 10), targetPosition)));
    server::WorldInputQueue queue;
    assert(queue.enqueueCombat(9101, 9102, 25.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    const Player* target = field->findPlayer(9102);
    assert(target != nullptr);
    assert(target->getStatus().getHp() == 75);
}

void rejects_out_of_range_combat_without_damage() {
    Field* field = Field::getInstance();
    const Position attackerPosition(9103, 0.0f, 0.0f, 0.0f);
    const Position targetPosition(9104, 101.0f, 0.0f, 0.0f);
    assert(field->setPlayer(Player(9103, Status(100, 10), attackerPosition)));
    assert(field->setPlayer(Player(9104, Status(100, 10), targetPosition)));
    server::WorldInputQueue queue;
    assert(queue.enqueueCombat(9103, 9104, 25.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(!applier.apply(frame, updates, error));
    const Player* target = field->findPlayer(9104);
    assert(target != nullptr);
    assert(target->getStatus().getHp() == 100);
}

void applies_spell_with_environment_conductivity_and_decay() {
    Field* field = Field::getInstance();
    const Position casterPosition(9201, 0.0f, 0.0f, 0.0f);
    const Position targetPosition(9202, 2.0f, 0.0f, 0.0f);
    assert(field->setPlayer(Player(9201, Status(100, 10), casterPosition)));
    assert(field->setPlayer(Player(9202, Status(100, 10), targetPosition)));
    server::WorldInputQueue queue;
    assert(queue.enqueueSpell(9201, 9202, "fire", 50.0f));
    server::WorldInputTick tick(queue);
    const server::WorldFrameInputs frame = tick.advanceFrame();
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    assert(field->environmentEther().value(world::EtherAttribute::Fire) > 4.0f);
    assert(field->environmentEther().value(world::EtherAttribute::Fire) < 5.0f);
}

void emits_hazard_event_when_environment_is_unstable() {
    Field* field = Field::getInstance();
    assert(field->environmentEther().add(world::EtherAttribute::Fire, 200.0f));
    assert(field->environmentEther().add(world::EtherAttribute::Earth, 200.0f));
    const server::WorldFrameInputs frame = {1, std::vector<server::WorldInput>()};
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.apply(frame, updates, error));
    assert(updates.size() == 1);
    assert(updates[0].payload.find("etherHazard=") == 0);
}

void captures_idle_poses_only_when_new_sessions_authenticate() {
    Field* field = Field::getInstance();
    const Status status;
    const int64_t idleId = 15212;
    const int64_t observerId = 15213;
    assert(field->setPlayer(Player(idleId, status, Position(idleId, 8.0f, 0.0f, 0.0f))));
    assert(field->setPlayer(
        Player(observerId, status, Position(observerId, 0.0f, 0.0f, 0.0f))));
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    std::vector<server::MovementAck> publishAcks;
    assert(applier.capturePublicSnapshotIfNewSessions(0, 11, updates,
                                                      publishAcks, error));
    assert(updates.empty());
    assert(applier.capturePublicSnapshotIfNewSessions(1, 11, updates,
                                                      publishAcks, error));
    assert(updates.size() == 1);
    assert(updates.back().kind == network::UpdateKind::Snapshot);
    assert(updates.back().payload.find("session=15212") != std::string::npos);
    assert(updates.back().payload.find("session=15213") != std::string::npos);
    assert(updates[0].payload.find("lastProcessedInputSequence") ==
           std::string::npos);
    assert(publishAcks.size() == applier.snapshotLocalAcks().size());
}

void captures_idle_public_player_poses_on_snapshot() {
    Field* field = Field::getInstance();
    const Status status;
    const int64_t idleId = 15210;
    const int64_t observerId = 15211;
    assert(field->setPlayer(Player(idleId, status, Position(idleId, 7.0f, 0.0f, 0.0f))));
    assert(field->setPlayer(
        Player(observerId, status, Position(observerId, 0.0f, 0.0f, 0.0f))));
    server::WorldFrameApplier applier(*field);
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(applier.capturePublicSnapshot(9, updates, error));
    assert(updates.size() == 1);
    const network::WorldUpdate& snapshot = updates.back();
    assert(snapshot.kind == network::UpdateKind::Snapshot);
    assert(snapshot.eventId == 0);
    assert(snapshot.worldTick == 9);
    assert(snapshot.payload.find("player.") != std::string::npos);
    assert(snapshot.payload.find("session=15210") != std::string::npos);
    assert(snapshot.payload.find("session=15211") != std::string::npos);
    assert(snapshot.payload.find("7") != std::string::npos);
    assert(snapshot.payload.find("lastProcessedInputSequence") ==
           std::string::npos);
    assert(snapshot.payload.find("local.") == std::string::npos);
    network::WorldUpdate observerCopy;
    assert(server::copyWorldUpdateForSession(snapshot, observerId,
                                             applier.snapshotLocalAcks(),
                                             observerCopy, error));
    assert(observerCopy.sequence == snapshot.sequence);
    assert(observerCopy.eventId == 0);
    assert(observerCopy.payload.find("local.x=") != std::string::npos);
    assert(observerCopy.payload.find("local.lastProcessedInputSequence=") !=
           std::string::npos);
    assert(observerCopy.payload.find(";x:") == std::string::npos);
    assert(observerCopy.payload.find("lastProcessedInputSequence:") ==
           std::string::npos);
}

} // namespace world_frame_applier_tests

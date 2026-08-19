#include <cassert>

#include "WorldFrameUpdateBuilder.h"

namespace world_frame_update_builder_tests {

void converts_actions_to_ordered_events() {
    ActionQueue queue;
    ServerTick tick(queue);
    const Status status;
    Player first;
    Player second;
    assert(tick.submit(Action(2, &first, nullptr, status)));
    assert(tick.submit(Action(2, &second, nullptr, status)));
    const FrameActions frame = tick.advanceFrame();
    server::WorldFrameUpdateBuilder builder;
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(builder.build(frame, updates, error));
    assert(updates.size() == 2);
    assert(updates[0].worldTick == 1);
    assert(updates[0].sequence < updates[1].sequence);
    assert(updates[0].eventId < updates[1].eventId);
    assert(network::validateWorldUpdate(updates[0], error));
}

void emits_no_update_for_empty_frame() {
    ActionQueue queue;
    ServerTick tick(queue);
    const FrameActions frame = tick.advanceFrame();
    server::WorldFrameUpdateBuilder builder;
    std::vector<network::WorldUpdate> updates;
    std::string error;
    assert(builder.build(frame, updates, error));
    assert(updates.empty());
    assert(builder.nextSequence() == 1);
}

void personalizes_owner_copy_without_changing_sequence() {
    const network::WorldUpdate published = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Event, 4, 7, 3,
        "movement=session:10;dx=1;dy=0;dz=0;clientInputSequence=1;inputSequence=1"};
    server::MovementAck ack;
    ack.sessionId = 10;
    ack.x = 1.0f;
    ack.y = 0.0f;
    ack.z = 0.0f;
    ack.worldTick = 4;
    ack.lastProcessedInputSequence = 1;
    std::vector<server::MovementAck> acks;
    acks.push_back(ack);
    network::WorldUpdate owner;
    network::WorldUpdate other;
    std::string error;
    assert(server::copyWorldUpdateForSession(published, 10, acks, owner, error));
    assert(server::copyWorldUpdateForSession(published, 20, acks, other, error));
    assert(owner.sequence == published.sequence);
    assert(other.sequence == published.sequence);
    assert(owner.eventId == published.eventId);
    assert(other.eventId == published.eventId);
    assert(other.payload.find("movement=session:10") == 0);
    assert(other.payload.find("lastProcessedInputSequence") == std::string::npos);
    assert(other.payload.find(";x:") == std::string::npos);
    assert(other.payload.find(";x=") != std::string::npos);
    assert(other.payload.find(";y=") != std::string::npos);
    assert(other.payload.find(";z=") != std::string::npos);
    assert(owner.payload.find("lastProcessedInputSequence:1") != std::string::npos);
}

void personalizes_snapshot_local_fields_without_changing_sequence() {
    const network::WorldUpdate published = {
        network::CURRENT_PROTOCOL_VERSION, network::UpdateKind::Snapshot, 8, 4, 0,
        "ether.fire=0;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=0;"
        "player.count=2;player.0.session=10;player.0.x=1;player.0.y=0;"
        "player.0.z=0;player.0.name=Alpha;player.1.session=20;player.1.x=5;"
        "player.1.y=0;player.1.z=0;player.1.name=Beta"};
    server::MovementAck observer;
    observer.sessionId = 20;
    observer.x = 5.0f;
    observer.y = 0.0f;
    observer.z = 0.0f;
    observer.worldTick = 8;
    observer.lastProcessedInputSequence = 0;
    std::vector<server::MovementAck> acks;
    acks.push_back(observer);
    network::WorldUpdate owner;
    network::WorldUpdate other;
    std::string error;
    assert(server::copyWorldUpdateForSession(published, 20, acks, owner, error));
    assert(server::copyWorldUpdateForSession(published, 10, acks, other, error));
    assert(owner.sequence == published.sequence);
    assert(other.sequence == published.sequence);
    assert(owner.eventId == 0);
    assert(other.eventId == 0);
    assert(owner.payload.find("local.x=") != std::string::npos);
    assert(owner.payload.find("local.lastProcessedInputSequence=0") !=
           std::string::npos);
    assert(other.payload.find("local.") == std::string::npos);
    assert(other.payload.find("player.0.session=10") != std::string::npos);
}

} // namespace world_frame_update_builder_tests

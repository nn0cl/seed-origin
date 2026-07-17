#include <cassert>

#include "ClientWorldUpdateReceiver.h"
#include "WorldUpdateFrameCodec.h"

namespace client_world_receiver_tests {

void applies_split_snapshot_and_hazard_event() {
    const network::WorldUpdate snapshot = {
        1, network::UpdateKind::Snapshot, 4, 10, 0,
        "ether.fire=1;ether.water=0;ether.earth=0;ether.air=0;ether.hazard=2"};
    const network::WorldUpdate event = {
        1, network::UpdateKind::Event, 5, 11, 7,
        "etherHazard=severity:2;instability:3"};
    std::vector<uint8_t> first;
    std::vector<uint8_t> second;
    std::string error;
    assert(network::encodeWorldUpdateFrame(snapshot, first, error));
    assert(network::encodeWorldUpdateFrame(event, second, error));
    first.insert(first.end(), second.begin(), second.end());
    client::ClientWorldUpdateReceiver receiver;
    size_t applied = 0;
    const std::vector<uint8_t> part(first.begin(), first.begin() + 7);
    assert(receiver.receive(part, applied, error) && applied == 0);
    const std::vector<uint8_t> remainder(first.begin() + 7, first.end());
    assert(receiver.receive(remainder, applied, error) && applied == 2);
    assert(receiver.environment().value().fire == 1.0f);
    assert(receiver.hazardEffects().size() == 1);
}

void rejects_sequence_gap_without_partial_application() {
    const network::WorldUpdate event = {
        1, network::UpdateKind::Event, 5, 2, 1,
        "etherHazard=severity:2;instability:3"};
    std::vector<uint8_t> frame;
    std::string error;
    assert(network::encodeWorldUpdateFrame(event, frame, error));
    client::ClientWorldUpdateReceiver receiver;
    size_t applied = 0;
    assert(!receiver.receive(frame, applied, error));
    assert(applied == 0 && receiver.hazardEffects().size() == 0);
    assert(receiver.snapshotRequested());
    assert(receiver.lastDecision() == client::WorldReceiveDecision::RequestSnapshot);
}

void requests_a_snapshot_after_reconnect_before_accepting_events() {
    client::ClientWorldUpdateReceiver receiver;
    receiver.beginReconnect();
    assert(receiver.snapshotRequested());
    const network::WorldUpdate event = {
        1, network::UpdateKind::Event, 1, 1, 1,
        "etherHazard=severity:2;instability:3"};
    std::vector<uint8_t> frame;
    std::string error;
    assert(network::encodeWorldUpdateFrame(event, frame, error));
    size_t applied = 0;
    assert(!receiver.receive(frame, applied, error));
    assert(applied == 0 && receiver.snapshotRequested());
}

} // namespace client_world_receiver_tests

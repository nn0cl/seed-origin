#include <cassert>

#include "MovementCommandHandler.h"

namespace movement_intent_queue_tests {

void queues_valid_movement_without_field_mutation() {
    server::MovementIntentQueue queue;
    server::MovementCommandHandler handler(queue);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Move, 42, "1,2,3"};
    std::string error;
    const server::MovementResult result = handler.handle(command);
    assert(result.accepted);
    assert(queue.pendingCount() == 1);
    const std::vector<server::MovementIntent> intents = queue.takeFrame();
    assert(intents.size() == 1);
    assert(intents[0].sessionId == 42);
    assert(intents[0].sequence == 1);
    assert(queue.pendingCount() == 0);
}

void rejects_invalid_session_without_queue_mutation() {
    server::MovementIntentQueue queue;
    server::MovementCommandHandler handler(queue);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Move, 0, "1,2,3"};
    const server::MovementResult result = handler.handle(command);
    assert(!result.accepted);
    assert(queue.pendingCount() == 0);
}

void restores_a_cut_frame_in_original_order() {
    server::MovementIntentQueue queue;
    assert(queue.enqueue(42, 1.0f, 0.0f, 0.0f));
    assert(queue.enqueue(42, 2.0f, 0.0f, 0.0f));
    const std::vector<server::MovementIntent> frame = queue.takeFrame();
    assert(queue.pendingCount() == 0);
    assert(queue.restoreFrame(frame));
    const std::vector<server::MovementIntent> restored = queue.takeFrame();
    assert(restored.size() == 2);
    assert(restored[0].sequence < restored[1].sequence);
    assert(restored[0].dx == 1.0f);
    assert(restored[1].dx == 2.0f);
}

} // namespace movement_intent_queue_tests

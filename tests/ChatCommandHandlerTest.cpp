#include <cassert>

#include "ChatCommandHandler.h"
#include "WorldFrameUpdateBuilder.h"

namespace chat_command_handler_tests {

void queues_authenticated_chat_and_preserves_sequence() {
    server::WorldInputQueue queue;
    server::ChatCommandHandler handler(queue);
    network::NetworkCommand command = {
        1, network::CommandType::Chat, 7, "world|hello"};
    std::string error;
    assert(handler.handle(command, error));
    const std::vector<server::WorldInput> inputs = queue.takeFrame();
    assert(inputs.size() == 1 && inputs[0].chat().senderId == 7);
    server::WorldFrameUpdateBuilder builder;
    server::WorldFrameInputs frame = {3, inputs};
    std::vector<network::WorldUpdate> updates;
    assert(builder.build(frame, updates, error));
    assert(updates.size() == 1 && updates[0].payload.find("chat=sender:7") == 0);
}

void rejects_control_text_and_missing_audience() {
    server::WorldInputQueue queue;
    server::ChatCommandHandler handler(queue);
    std::string error;
    network::NetworkCommand control = {1, network::CommandType::Chat, 7,
                                       "world|bad\x01text"};
    assert(!handler.handle(control, error));
    network::NetworkCommand missing = {1, network::CommandType::Chat, 7, "hello"};
    assert(!handler.handle(missing, error));
    assert(queue.pendingCount() == 0);
}

} // namespace chat_command_handler_tests

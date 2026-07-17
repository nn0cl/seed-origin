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

} // namespace world_frame_update_builder_tests

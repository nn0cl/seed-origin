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

} // namespace world_frame_applier_tests

#include <cassert>

#include "Action.h"

namespace action_input_tests {

void rejects_missing_players_for_target_action() {
    Status status(1, 1);
    Action action(1, nullptr, nullptr, status);
    assert(!action.isValid());
}

void permits_field_action_without_players() {
    Status status(1, 1);
    Action action(0, nullptr, nullptr, status);
    assert(action.isValid());
}

void rejects_unknown_action_type() {
    Player player;
    Status status(1, 1);
    Action action(99, &player, &player, status);
    assert(!action.isValid());
}

} // namespace action_input_tests

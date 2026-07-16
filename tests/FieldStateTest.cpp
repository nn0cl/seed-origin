#include <cassert>

#include "Field.h"

namespace field_state_tests {

void removes_players_by_full_id() {
    Status status(10, 10);
    const int64_t playerId = 4294967297LL;
    Position position(playerId, 0.0f, 0.0f, 0.0f);
    Player player(playerId, status, position);
    assert(Field::setPlayer(player));
    assert(Field::unsetPlayer(player));
    assert(!Field::unsetPlayer(player));
}

void accepts_empty_frame() {
    Field::getInstance()->processFrame();
}

} // namespace field_state_tests

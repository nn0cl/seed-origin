#include <cassert>

#include "MovementCommandHandler.h"

namespace movement_command_handler_tests {

void rejects_malformed_or_unknown_move() {
    Field* field = Field::getInstance();
    server::MovementCommandHandler handler(*field);
    const network::NetworkCommand malformed = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Move, 1, "1,2"};
    const server::MovementResult result = handler.handle(malformed);
    assert(!result.accepted);
}

void accepts_bounded_move_for_existing_player() {
    Field* field = Field::getInstance();
    const int64_t playerId = 42;
    const Status status;
    const Position position(playerId, 0.0f, 0.0f, 0.0f);
    assert(field->setPlayer(Player(playerId, status, position)));
    server::MovementCommandHandler handler(*field);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Move, playerId, "1,2,3"};
    const server::MovementResult result = handler.handle(command);
    assert(result.accepted);
}

} // namespace movement_command_handler_tests

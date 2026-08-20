#include <cassert>
#include <string>
#include <vector>

#include "AuthenticatedPlayerIdPort.h"
#include "DisconnectCommandHandler.h"
#include "Field.h"
#include "FieldSessionPresence.h"
#include "LoginFieldSpawnSettings.h"
#include "Player.h"
#include "Position.h"
#include "Status.h"

namespace disconnect_command_handler_tests {
namespace {

class FixedPlayerIdPort : public server::AuthenticatedPlayerIdPort {
public:
    explicit FixedPlayerIdPort(int64_t playerId) : playerId(playerId) {}
    bool resolvePlayerId(int64_t, const std::string&, int64_t& out) const {
        out = playerId;
        return playerId > 0;
    }

private:
    int64_t playerId;
};

void clearFieldPlayers() {
    Field* field = Field::getInstance();
    const std::vector<int64_t> ids = field->residentPlayerIds();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        Field::unsetPlayer(Player(ids[i], Status(), Position(ids[i], 0, 0, 0)));
    }
    server::FieldSessionPresence::usePlayerIdPort(0);
    server::FieldSessionPresence::useSpawnSettings(
        server::LoginFieldSpawnSettings());
}

}

void ends_active_session_without_unsetting_the_player() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    session::SessionRegistry registry;
    const session::SessionInfo session = registry.login("alice");
    assert(server::FieldSessionPresence::placeAfterLogin(session.internalId,
                                                         "alice"));
    Field* field = Field::getInstance();
    const int64_t gameplayId = field->findPlayer(session.internalId)->getPlayerId();
    assert(!field->publicPlayerPoses().empty());

    server::DisconnectCommandHandler handler(registry);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Disconnect,
        session.internalId, ""};
    const server::DisconnectResult result = handler.handle(command);
    assert(result.accepted);
    assert(!registry.isActive(session.internalId));
    assert(!field->hasPlayer(session.internalId));
    assert(field->hasPlayer(gameplayId));
    assert(field->findPlayer(gameplayId)->getAuthPlayerId() == 9001);
    assert(field->findPlayer(gameplayId)->getPlayerId() == gameplayId);
    assert(field->publicPlayerPoses().empty());
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9002, "Hero"));

    const session::SessionInfo rebound = registry.login("alice");
    assert(server::FieldSessionPresence::placeAfterLogin(rebound.internalId,
                                                         "alice"));
    const Player* placed = field->findPlayer(rebound.internalId);
    assert(placed != 0);
    assert(placed->getPlayerId() == gameplayId);
    assert(placed->getAuthPlayerId() == 9001);
    assert(placed->getPlayerName() == "Hero");
    clearFieldPlayers();
}

void rejects_inactive_or_missing_session() {
    session::SessionRegistry registry;
    server::DisconnectCommandHandler handler(registry);
    const network::NetworkCommand unknown = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Disconnect, 21,
        ""};
    const server::DisconnectResult missing = handler.handle(unknown);
    assert(!missing.accepted);
    assert(missing.error == "disconnect requires an active anonymous session");

    const network::NetworkCommand unauthenticated = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Disconnect, 0,
        ""};
    const server::DisconnectResult invalid = handler.handle(unauthenticated);
    assert(!invalid.accepted);
    assert(invalid.error == "command requires an active session id");
}

void rejects_non_empty_disconnect_payload() {
    session::SessionRegistry registry;
    const session::SessionInfo session = registry.login("alice");
    server::DisconnectCommandHandler handler(registry);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Disconnect,
        session.internalId, "extra"};
    const server::DisconnectResult result = handler.handle(command);
    assert(!result.accepted);
    assert(result.error == "command payload must be empty");
}

} // namespace disconnect_command_handler_tests

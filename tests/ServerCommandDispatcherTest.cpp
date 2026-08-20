#include "ServerCommandDispatcher.h"

#include <cassert>

#include "AuthenticatedPlayerIdPort.h"
#include "Field.h"
#include "FieldSessionPresence.h"
#include "LoginFieldSpawnSettings.h"
#include "Player.h"
#include "Position.h"
#include "Status.h"

namespace server_command_dispatcher_tests {
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

void accepts_login() {
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0, "player"};

    const server::CommandDispatchResult result = dispatcher.dispatch(command);
    assert(result.accepted);
    assert(result.session.internalId != 0);
}

void rejects_unimplemented_command() {
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION,
        static_cast<network::CommandType>(99), 1, ""};

    const server::CommandDispatchResult result = dispatcher.dispatch(command);
    assert(!result.accepted);
    assert(result.error == "command handler is not implemented");
}

void disconnects_active_session_and_omits_public_pose() {
    clearFieldPlayers();
    const FixedPlayerIdPort port(9001);
    server::FieldSessionPresence::usePlayerIdPort(&port);
    assert(server::FieldSessionPresence::operatorSetPlayerName(9001, "Hero"));
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    const session::SessionInfo session = registry.login("alice");
    assert(server::FieldSessionPresence::placeAfterLogin(session.internalId,
                                                         "alice"));
    Field* field = Field::getInstance();
    const int64_t gameplayId = field->findPlayer(session.internalId)->getPlayerId();

    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Disconnect,
        session.internalId, ""};
    const server::CommandDispatchResult result = dispatcher.dispatch(command);
    assert(result.accepted);
    assert(!registry.isActive(session.internalId));
    assert(!field->hasPlayer(session.internalId));
    assert(field->hasPlayer(gameplayId));
    assert(field->publicPlayerPoses().empty());
    assert(!server::FieldSessionPresence::operatorSetPlayerName(9002, "Hero"));
    clearFieldPlayers();
}

void rejects_disconnect_without_an_active_session() {
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry);
    const network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Disconnect, 1,
        ""};
    const server::CommandDispatchResult result = dispatcher.dispatch(command);
    assert(!result.accepted);
    assert(result.error == "disconnect requires an active anonymous session");
}

void routes_combat_only_for_an_active_session() {
    session::SessionRegistry registry;
    server::WorldInputQueue queue;
    server::ServerCommandDispatcher dispatcher(registry, queue);
    const session::SessionInfo session = registry.login("player");
    const network::NetworkCommand attack = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Attack,
        session.internalId, "attack-1|2,100"};
    const server::CommandDispatchResult accepted = dispatcher.dispatch(attack);
    assert(accepted.accepted);
    assert(queue.pendingCount() == 1);

    const network::NetworkCommand forged = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::CastSpell,
        9999, "spell-1|2,fire,100"};
    const server::CommandDispatchResult rejected = dispatcher.dispatch(forged);
    assert(!rejected.accepted);
    assert(queue.pendingCount() == 1);
}

} // namespace server_command_dispatcher_tests

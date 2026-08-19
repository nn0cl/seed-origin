#include "FieldSessionPresence.h"

#include <map>

#include "Field.h"
#include "InMemoryAuthenticatedPlayerIdStub.h"
#include "Player.h"
#include "Position.h"
#include "Status.h"

namespace server {
namespace {

const AuthenticatedPlayerIdPort* playerIdPort = 0;
LoginFieldSpawnSettings currentSpawnSettings;
InMemoryAuthenticatedPlayerIdStub defaultPlayerIdStub;
std::map<int64_t, std::string> operatorNamesByAuthId;

const AuthenticatedPlayerIdPort& activePlayerIdPort() {
    if (playerIdPort != 0) return *playerIdPort;
    return defaultPlayerIdStub;
}

}

void FieldSessionPresence::usePlayerIdPort(const AuthenticatedPlayerIdPort* port) {
    playerIdPort = port;
    if (port == 0) operatorNamesByAuthId.clear();
}

void FieldSessionPresence::useSpawnSettings(const LoginFieldSpawnSettings& settings) {
    currentSpawnSettings = settings;
}

LoginFieldSpawnSettings FieldSessionPresence::spawnSettings() {
    return currentSpawnSettings;
}

bool FieldSessionPresence::placeAfterLogin(int64_t sessionId) {
    return placeAfterLogin(sessionId, std::string(), currentSpawnSettings);
}

bool FieldSessionPresence::placeAfterLogin(int64_t sessionId,
                                           const std::string& claimedId) {
    return placeAfterLogin(sessionId, claimedId, currentSpawnSettings);
}

bool FieldSessionPresence::placeAfterLogin(
    int64_t sessionId, const std::string& claimedId,
    const LoginFieldSpawnSettings& settings) {
    if (sessionId <= 0) return false;
    int64_t authPlayerId = 0;
    if (!activePlayerIdPort().resolvePlayerId(sessionId, claimedId, authPlayerId) ||
        authPlayerId <= 0) {
        return false;
    }

    Field* field = Field::getInstance();
    Player* existing = field->findPlayerByAuthId(authPlayerId);
    if (existing != 0) {
        return field->bindSession(sessionId, existing->getPlayerId());
    }

    std::string displayName;
    const std::map<int64_t, std::string>::const_iterator assigned =
        operatorNamesByAuthId.find(authPlayerId);
    if (assigned != operatorNamesByAuthId.end()) {
        displayName = assigned->second;
    } else {
        displayName = settings.playerName;
    }
    if (displayName.empty() || field->hasPlayerName(displayName)) return false;

    const int64_t gameplayId = field->allocateGameplayId();
    if (gameplayId <= 0) return false;

    const Status status(settings.clampedHp(), settings.clampedMp());
    const Position pose(gameplayId, settings.x, settings.y, settings.z);
    Player player(gameplayId, status, pose);
    player.setAuthPlayerId(authPlayerId);
    if (!player.setPlayerName(displayName)) return false;
    if (!Field::setPlayer(player)) return false;
    operatorNamesByAuthId[authPlayerId] = displayName;
    return field->bindSession(sessionId, gameplayId);
}

bool FieldSessionPresence::operatorSetPlayerName(int64_t authPlayerId,
                                                 const std::string& displayName) {
    if (authPlayerId <= 0) return false;
    Field* field = Field::getInstance();
    Player* existing = field->findPlayerByAuthId(authPlayerId);
    if (existing != 0) {
        if (existing->getPlayerName() != displayName &&
            field->hasPlayerName(displayName)) {
            return false;
        }
        if (!existing->setPlayerName(displayName)) return false;
    } else if (field->hasPlayerName(displayName)) {
        return false;
    } else {
        Player probe;
        if (!probe.setPlayerName(displayName)) return false;
    }
    operatorNamesByAuthId[authPlayerId] = displayName;
    return true;
}

bool FieldSessionPresence::playerSetPlayerName(int64_t sessionId,
                                               const std::string& displayName) {
    (void)sessionId;
    (void)displayName;
    return false;
}

bool FieldSessionPresence::removeAfterLogout(int64_t sessionId) {
    if (sessionId <= 0) return false;
    return Field::getInstance()->unbindSession(sessionId);
}

bool FieldSessionPresence::releaseAfterStop(int64_t sessionId) {
    if (sessionId <= 0) return false;
    Field* field = Field::getInstance();
    const int64_t gameplayId = field->playerIdForSession(sessionId);
    field->unbindSession(sessionId);
    if (gameplayId <= 0) return true;
    return Field::unsetPlayer(
        Player(gameplayId, Status(), Position(gameplayId, 0.0f, 0.0f, 0.0f)));
}

}

#include "FieldSessionPresence.h"

#include "Field.h"
#include "Player.h"
#include "Position.h"
#include "Status.h"

namespace server {

bool FieldSessionPresence::placeAfterLogin(int64_t sessionId) {
    if (sessionId <= 0) return false;
    Field* field = Field::getInstance();
    if (field->hasPlayer(sessionId)) return true;
    const Status status;
    const Position origin(sessionId, 0.0f, 0.0f, 0.0f);
    return Field::setPlayer(Player(sessionId, status, origin));
}

bool FieldSessionPresence::removeAfterLogout(int64_t sessionId) {
    if (sessionId <= 0) return false;
    const Status status;
    const Position origin(sessionId, 0.0f, 0.0f, 0.0f);
    return Field::unsetPlayer(Player(sessionId, status, origin));
}

}

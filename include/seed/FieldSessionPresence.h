#ifndef SEED_FIELD_SESSION_PRESENCE_H
#define SEED_FIELD_SESSION_PRESENCE_H

#include <cstdint>

namespace server {

// Temporary Login presence on Field until LISS-0153 defines durable spawn.
// PlayerId equals session.internalId. Pose is origin (0,0,0). Status() is
// the default constructor used by existing Snapshot Field tests.
class FieldSessionPresence {
public:
    static bool placeAfterLogin(int64_t sessionId);
    static bool removeAfterLogout(int64_t sessionId);
};

}

#endif

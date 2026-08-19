#ifndef SEED_FIELD_SESSION_PRESENCE_H
#define SEED_FIELD_SESSION_PRESENCE_H

#include <cstdint>
#include <string>

#include "AuthenticatedPlayerIdPort.h"
#include "LoginFieldSpawnSettings.h"

namespace server {

class FieldSessionPresence {
public:
    static void usePlayerIdPort(const AuthenticatedPlayerIdPort* port);
    static void useSpawnSettings(const LoginFieldSpawnSettings& settings);
    static LoginFieldSpawnSettings spawnSettings();

    static bool placeAfterLogin(int64_t sessionId);
    static bool placeAfterLogin(int64_t sessionId, const std::string& claimedId);
    static bool placeAfterLogin(int64_t sessionId, const std::string& claimedId,
                                const LoginFieldSpawnSettings& settings);
    static bool removeAfterLogout(int64_t sessionId);
    static bool releaseAfterStop(int64_t sessionId);
};

}

#endif

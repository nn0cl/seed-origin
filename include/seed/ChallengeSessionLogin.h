#ifndef SEED_CHALLENGE_SESSION_LOGIN_H
#define SEED_CHALLENGE_SESSION_LOGIN_H

#include <cstdint>
#include <string>

#include "ChallengeSessionPorts.h"

namespace server {

struct ChallengeLoginResult {
    bool accepted;
    std::string reason;
    PlayerSessionKey sessionKey;
    int64_t userId;
    int64_t expiresAtUnix;
};

struct KeepAliveResult {
    bool accepted;
    std::string reason;
    int64_t expiresAtUnix;
};

// Application service: claim a ChallengeKey, issue a 30-minute PlayerSessionKey,
// extend via keep-alive, and validate for reconnect.
class ChallengeSessionLoginService {
public:
    static const int64_t kSessionTtlSeconds = 30 * 60;

    ChallengeSessionLoginService(PlayerChallengeClaimPort& challenges,
                                 PlayerSessionStorePort& sessions,
                                 SessionKeyIssuer& keys,
                                 WallClock& clock);

    ChallengeLoginResult loginWithChallenge(const ChallengeKey& challenge);
    KeepAliveResult keepAlive(const PlayerSessionKey& sessionKey);
    bool validateSession(const PlayerSessionKey& sessionKey) const;

private:
    int64_t expiryFrom(int64_t nowUnixSeconds) const;

    static ChallengeLoginResult rejectedLogin(const std::string& reason);
    static ChallengeLoginResult acceptedLogin(int64_t userId,
                                              const PlayerSessionKey& sessionKey,
                                              int64_t expiresAtUnix);
    static KeepAliveResult rejectedKeepAlive(const std::string& reason);
    static KeepAliveResult acceptedKeepAlive(int64_t expiresAtUnix);

    PlayerChallengeClaimPort& challenges;
    PlayerSessionStorePort& sessions;
    SessionKeyIssuer& keys;
    WallClock& clock;
};

}

#endif

#include "ChallengeSessionLogin.h"

namespace server {

const int64_t ChallengeSessionLoginService::kSessionTtlSeconds;

ChallengeSessionLoginService::ChallengeSessionLoginService(
    PlayerChallengeClaimPort& challenges,
    PlayerSessionStorePort& sessions,
    SessionKeyIssuer& keys,
    WallClock& clock)
    : challenges(challenges),
      sessions(sessions),
      keys(keys),
      clock(clock) {}

ChallengeLoginResult ChallengeSessionLoginService::loginWithChallenge(
    const ChallengeKey& challenge) {
    ChallengeLoginResult result;
    result.accepted = false;
    result.userId = 0;
    result.expiresAtUnix = 0;

    const int64_t now = clock.nowUnixSeconds();
    int64_t userId = 0;
    if (!challenges.claim(challenge, now, userId)) {
        result.reason = "invalid_challenge";
        return result;
    }

    const PlayerSessionKey sessionKey = keys.issue();
    const int64_t expiresAt = now + kSessionTtlSeconds;
    sessions.create(userId, sessionKey, expiresAt);

    result.accepted = true;
    result.sessionKey = sessionKey;
    result.userId = userId;
    result.expiresAtUnix = expiresAt;
    return result;
}

KeepAliveResult ChallengeSessionLoginService::keepAlive(
    const PlayerSessionKey& sessionKey) {
    KeepAliveResult result;
    result.accepted = false;
    result.expiresAtUnix = 0;

    const int64_t now = clock.nowUnixSeconds();
    const int64_t newExpiresAt = now + kSessionTtlSeconds;
    if (!sessions.extend(sessionKey, now, newExpiresAt)) {
        result.reason = "invalid_session";
        return result;
    }

    result.accepted = true;
    result.expiresAtUnix = newExpiresAt;
    return result;
}

bool ChallengeSessionLoginService::validateSession(
    const PlayerSessionKey& sessionKey) const {
    return sessions.isActive(sessionKey, clock.nowUnixSeconds());
}

}

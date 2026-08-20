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
    const int64_t now = clock.nowUnixSeconds();
    int64_t userId = 0;
    if (!challenges.claim(challenge, now, userId)) {
        return rejectedLogin("invalid_challenge");
    }

    const PlayerSessionKey sessionKey = keys.issue();
    const int64_t expiresAt = expiryFrom(now);
    sessions.create(userId, sessionKey, expiresAt);
    return acceptedLogin(userId, sessionKey, expiresAt);
}

KeepAliveResult ChallengeSessionLoginService::keepAlive(
    const PlayerSessionKey& sessionKey) {
    const int64_t now = clock.nowUnixSeconds();
    const int64_t newExpiresAt = expiryFrom(now);
    if (!sessions.extend(sessionKey, now, newExpiresAt)) {
        return rejectedKeepAlive("invalid_session");
    }
    return acceptedKeepAlive(newExpiresAt);
}

bool ChallengeSessionLoginService::validateSession(
    const PlayerSessionKey& sessionKey) const {
    return sessions.isActive(sessionKey, clock.nowUnixSeconds());
}

int64_t ChallengeSessionLoginService::expiryFrom(int64_t nowUnixSeconds) const {
    return nowUnixSeconds + kSessionTtlSeconds;
}

ChallengeLoginResult ChallengeSessionLoginService::rejectedLogin(
    const std::string& reason) {
    ChallengeLoginResult result;
    result.accepted = false;
    result.reason = reason;
    result.userId = 0;
    result.expiresAtUnix = 0;
    return result;
}

ChallengeLoginResult ChallengeSessionLoginService::acceptedLogin(
    int64_t userId,
    const PlayerSessionKey& sessionKey,
    int64_t expiresAtUnix) {
    ChallengeLoginResult result;
    result.accepted = true;
    result.sessionKey = sessionKey;
    result.userId = userId;
    result.expiresAtUnix = expiresAtUnix;
    return result;
}

KeepAliveResult ChallengeSessionLoginService::rejectedKeepAlive(
    const std::string& reason) {
    KeepAliveResult result;
    result.accepted = false;
    result.reason = reason;
    result.expiresAtUnix = 0;
    return result;
}

KeepAliveResult ChallengeSessionLoginService::acceptedKeepAlive(
    int64_t expiresAtUnix) {
    KeepAliveResult result;
    result.accepted = true;
    result.expiresAtUnix = expiresAtUnix;
    return result;
}

}

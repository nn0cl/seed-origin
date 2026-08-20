#ifndef SEED_CHALLENGE_SESSION_LOGIN_H
#define SEED_CHALLENGE_SESSION_LOGIN_H

#include <cstdint>
#include <string>

namespace server {

// LISS-0147 Phase 1 Red contract: seed_server claims a ChallengeKey and issues
// a PlayerSessionKey without ever seeing passwords or the users table.

struct ChallengeKey {
    std::string value;
};

struct PlayerSessionKey {
    std::string value;
};

class WallClock {
public:
    virtual ~WallClock() {}
    virtual int64_t nowUnixSeconds() const = 0;
};

class PlayerChallengeClaimPort {
public:
    virtual ~PlayerChallengeClaimPort() {}

    // Atomically claim a valid, unclaimed, unexpired challenge.
    // Returns false when missing, expired, or already claimed.
    virtual bool claim(const ChallengeKey& key,
                       int64_t nowUnixSeconds,
                       int64_t& outUserId) = 0;
};

class PlayerSessionStorePort {
public:
    virtual ~PlayerSessionStorePort() {}
    virtual void create(int64_t userId,
                        const PlayerSessionKey& key,
                        int64_t expiresAtUnix) = 0;
    virtual bool extend(const PlayerSessionKey& key,
                        int64_t nowUnixSeconds,
                        int64_t newExpiresAtUnix) = 0;
    virtual bool isActive(const PlayerSessionKey& key,
                          int64_t nowUnixSeconds) const = 0;
};

class SessionKeyIssuer {
public:
    virtual ~SessionKeyIssuer() {}
    virtual PlayerSessionKey issue() = 0;
};

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
    PlayerChallengeClaimPort& challenges;
    PlayerSessionStorePort& sessions;
    SessionKeyIssuer& keys;
    WallClock& clock;
};

}

#endif

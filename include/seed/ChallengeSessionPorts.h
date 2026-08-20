#ifndef SEED_CHALLENGE_SESSION_PORTS_H
#define SEED_CHALLENGE_SESSION_PORTS_H

#include <cstdint>
#include <string>

namespace server {

// Ports for LISS-0147: seed_server claims ChallengeKeys and stores
// PlayerSessionKeys without reading passwords or the users table.

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

}

#endif

#ifndef SEED_CHALLENGE_AUTH_TEST_FIXTURES_H
#define SEED_CHALLENGE_AUTH_TEST_FIXTURES_H

#include <map>
#include <string>

#include "AuthenticatedPlayerIdPort.h"
#include "ChallengeSessionLogin.h"

namespace seed_test {

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

class FixedWallClock : public server::WallClock {
public:
    explicit FixedWallClock(int64_t now) : nowUnix(now) {}
    int64_t nowUnixSeconds() const { return nowUnix; }
    int64_t nowUnix;
};

class FakeChallengeClaimPort : public server::PlayerChallengeClaimPort {
public:
    struct Row {
        int64_t userId;
        int64_t expiresAtUnix;
        bool claimed;
    };

    std::map<std::string, Row> rows;

    bool claim(const server::ChallengeKey& key,
               int64_t nowUnixSeconds,
               int64_t& outUserId) {
        std::map<std::string, Row>::iterator it = rows.find(key.value);
        if (it == rows.end() || it->second.claimed ||
            it->second.expiresAtUnix <= nowUnixSeconds) {
            return false;
        }
        it->second.claimed = true;
        outUserId = it->second.userId;
        return true;
    }

    void putUnclaimed(const std::string& key, int64_t userId, int64_t expiresAt) {
        Row row;
        row.userId = userId;
        row.expiresAtUnix = expiresAt;
        row.claimed = false;
        rows[key] = row;
    }
};

class FakeSessionStore : public server::PlayerSessionStorePort {
public:
    struct Row {
        int64_t userId;
        int64_t expiresAtUnix;
    };

    std::map<std::string, Row> rows;

    void create(int64_t userId,
                const server::PlayerSessionKey& key,
                int64_t expiresAtUnix) {
        Row row;
        row.userId = userId;
        row.expiresAtUnix = expiresAtUnix;
        rows[key.value] = row;
    }

    bool extend(const server::PlayerSessionKey&, int64_t, int64_t) { return false; }
    bool isActive(const server::PlayerSessionKey&, int64_t) const { return false; }
};

class FixedKeyIssuer : public server::SessionKeyIssuer {
public:
    explicit FixedKeyIssuer(const std::string& value) : nextValue(value), counter(0) {}

    server::PlayerSessionKey issue() {
        server::PlayerSessionKey key;
        key.value = nextValue + "-" + std::to_string(++counter);
        return key;
    }

    std::string nextValue;
    int counter;
};

}

#endif

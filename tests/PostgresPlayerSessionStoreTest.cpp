#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

#include <pqxx/pqxx>

#include "ChallengeSessionLogin.h"
#include "PostgresPlayerSessionStore.h"

namespace {

const char* kSkipMessage =
    "SEED_IDENTITY_DB_URL not set; skipping Postgres player session test\n";

std::unique_ptr<server::PostgresPlayerSessionStore> openStoreOrSkip() {
    std::unique_ptr<server::PostgresPlayerSessionStore> store =
        server::PostgresPlayerSessionStore::fromEnvironment();
    if (!store) {
        std::cerr << kSkipMessage;
    }
    return store;
}

int64_t ensureTestUser(pqxx::connection& connection, const std::string& username) {
    pqxx::work tx(connection);
    const pqxx::result inserted = tx.exec(
        "INSERT INTO users (username, password_hash) "
        "VALUES ($1, crypt('test-only', gen_salt('bf'))) "
        "ON CONFLICT (username) DO UPDATE SET username = EXCLUDED.username "
        "RETURNING id",
        pqxx::params{username});
    tx.commit();
    return inserted[0]["id"].as<int64_t>();
}

void clearChallenge(pqxx::connection& connection, const std::string& key) {
    pqxx::work tx(connection);
    tx.exec("DELETE FROM player_challenges WHERE challenge_key = $1",
            pqxx::params{key});
    tx.commit();
}

void insertChallenge(pqxx::connection& connection,
                     const std::string& key,
                     int64_t userId,
                     int64_t expiresAtUnix,
                     bool claimed) {
    clearChallenge(connection, key);
    pqxx::work tx(connection);
    if (claimed) {
        tx.exec(
            "INSERT INTO player_challenges "
            "(challenge_key, user_id, expires_at, claimed_at) "
            "VALUES ($1, $2, to_timestamp($3), now())",
            pqxx::params{key, userId, expiresAtUnix});
    } else {
        tx.exec(
            "INSERT INTO player_challenges (challenge_key, user_id, expires_at) "
            "VALUES ($1, $2, to_timestamp($3))",
            pqxx::params{key, userId, expiresAtUnix});
    }
    tx.commit();
}

void clearSession(pqxx::connection& connection, const std::string& token) {
    pqxx::work tx(connection);
    tx.exec("DELETE FROM player_sessions WHERE session_token = $1",
            pqxx::params{token});
    tx.commit();
}

class FixedWallClock : public server::WallClock {
public:
    explicit FixedWallClock(int64_t now) : nowUnix(now) {}
    int64_t nowUnixSeconds() const { return nowUnix; }
    int64_t nowUnix;
};

class FixedKeyIssuer : public server::SessionKeyIssuer {
public:
    explicit FixedKeyIssuer(const std::string& value) : nextValue(value) {}

    server::PlayerSessionKey issue() {
        server::PlayerSessionKey key;
        key.value = nextValue;
        return key;
    }

    std::string nextValue;
};

} // namespace

namespace postgres_player_session_store_tests {

void claims_valid_unexpired_challenge() {
    std::unique_ptr<server::PostgresPlayerSessionStore> store = openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(connectionString);
    const int64_t userId = ensureTestUser(connection, "postgres-challenge-user");
    const int64_t now = 1700000000;
    const std::string challengeKey = "postgres-challenge-valid";
    insertChallenge(connection, challengeKey, userId, now + 120, false);

    int64_t claimedUserId = 0;
    server::ChallengeKey key;
    key.value = challengeKey;
    assert(store->claim(key, now, claimedUserId));
    assert(claimedUserId == userId);

    clearChallenge(connection, challengeKey);
}

void rejects_expired_challenge() {
    std::unique_ptr<server::PostgresPlayerSessionStore> store = openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(connectionString);
    const int64_t userId = ensureTestUser(connection, "postgres-challenge-expired");
    const int64_t now = 1700000000;
    const std::string challengeKey = "postgres-challenge-expired";
    insertChallenge(connection, challengeKey, userId, now - 1, false);

    int64_t claimedUserId = 0;
    server::ChallengeKey key;
    key.value = challengeKey;
    assert(!store->claim(key, now, claimedUserId));
    assert(claimedUserId == 0);

    clearChallenge(connection, challengeKey);
}

void rejects_already_claimed_challenge() {
    std::unique_ptr<server::PostgresPlayerSessionStore> store = openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(connectionString);
    const int64_t userId = ensureTestUser(connection, "postgres-challenge-claimed");
    const int64_t now = 1700000000;
    const std::string challengeKey = "postgres-challenge-claimed";
    insertChallenge(connection, challengeKey, userId, now + 120, true);

    int64_t claimedUserId = 0;
    server::ChallengeKey key;
    key.value = challengeKey;
    assert(!store->claim(key, now, claimedUserId));

    clearChallenge(connection, challengeKey);
}

void login_use_case_creates_player_session_row() {
    std::unique_ptr<server::PostgresPlayerSessionStore> store = openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(connectionString);
    const int64_t userId = ensureTestUser(connection, "postgres-challenge-usecase");
    const int64_t now = 1700000000;
    const std::string challengeKey = "postgres-challenge-usecase";
    const std::string sessionToken = "postgres-session-token-1";
    insertChallenge(connection, challengeKey, userId, now + 120, false);
    clearSession(connection, sessionToken);

    FixedWallClock clock(now);
    FixedKeyIssuer keys(sessionToken);
    server::ChallengeSessionLoginService auth(*store, *store, keys, clock);

    server::ChallengeKey key;
    key.value = challengeKey;
    const server::ChallengeLoginResult result = auth.loginWithChallenge(key);
    assert(result.accepted);
    assert(result.userId == userId);
    assert(result.sessionKey.value == sessionToken);

    pqxx::work tx(connection);
    const pqxx::result rows = tx.exec(
        "SELECT user_id, extract(epoch from expires_at)::bigint AS expires_at "
        "FROM player_sessions WHERE session_token = $1",
        pqxx::params{sessionToken});
    tx.commit();
    assert(!rows.empty());
    assert(rows[0]["user_id"].as<int64_t>() == userId);
    assert(rows[0]["expires_at"].as<int64_t>() == now + server::ChallengeSessionLoginService::kSessionTtlSeconds);

    clearSession(connection, sessionToken);
    clearChallenge(connection, challengeKey);
}

void extends_active_session() {
    std::unique_ptr<server::PostgresPlayerSessionStore> store = openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(connectionString);
    const int64_t userId = ensureTestUser(connection, "postgres-session-extend");
    const int64_t now = 1700000000;
    const std::string sessionToken = "postgres-session-extend";
    clearSession(connection, sessionToken);

    store->create(userId, server::PlayerSessionKey{sessionToken}, now + 60);

    const int64_t extendedUntil = now + server::ChallengeSessionLoginService::kSessionTtlSeconds;
    server::PlayerSessionKey key;
    key.value = sessionToken;
    assert(store->extend(key, now, extendedUntil));
    assert(store->isActive(key, now));

    pqxx::work tx(connection);
    const pqxx::result rows = tx.exec(
        "SELECT extract(epoch from expires_at)::bigint AS expires_at "
        "FROM player_sessions WHERE session_token = $1",
        pqxx::params{sessionToken});
    tx.commit();
    assert(!rows.empty());
    assert(rows[0]["expires_at"].as<int64_t>() == extendedUntil);

    clearSession(connection, sessionToken);
}

void is_active_rejects_expired_session() {
    std::unique_ptr<server::PostgresPlayerSessionStore> store = openStoreOrSkip();
    if (!store) {
        return;
    }

    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    pqxx::connection connection(connectionString);
    const int64_t userId = ensureTestUser(connection, "postgres-session-expired");
    const int64_t now = 1700000000;
    const std::string sessionToken = "postgres-session-expired";
    clearSession(connection, sessionToken);

    pqxx::work tx(connection);
    tx.exec(
        "INSERT INTO player_sessions (session_token, user_id, expires_at) "
        "VALUES ($1, $2, to_timestamp($3))",
        pqxx::params{sessionToken, userId, now - 1});
    tx.commit();

    server::PlayerSessionKey key;
    key.value = sessionToken;
    assert(!store->isActive(key, now));

    clearSession(connection, sessionToken);
}

} // namespace postgres_player_session_store_tests

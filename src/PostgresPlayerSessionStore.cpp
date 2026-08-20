#include "PostgresPlayerSessionStore.h"

#include <cstdlib>

#include <pqxx/pqxx>

namespace server {

namespace {

bool hasConnectionString(const char* connectionString) {
    return connectionString != nullptr && connectionString[0] != '\0';
}

const char* kActiveSessionPredicate =
    "WHERE session_token = $1 "
    "  AND revoked_at IS NULL "
    "  AND expires_at > to_timestamp($2)";

}

PostgresPlayerSessionStore::PostgresPlayerSessionStore(
    const std::string& connectionString)
    : connection(std::make_unique<pqxx::connection>(connectionString)) {}

PostgresPlayerSessionStore::~PostgresPlayerSessionStore() = default;

std::unique_ptr<PostgresPlayerSessionStore> PostgresPlayerSessionStore::fromEnvironment() {
    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    if (!hasConnectionString(connectionString)) {
        return nullptr;
    }
    return std::make_unique<PostgresPlayerSessionStore>(std::string(connectionString));
}

bool PostgresPlayerSessionStore::claim(const ChallengeKey& key,
                                       int64_t nowUnixSeconds,
                                       int64_t& outUserId) {
    outUserId = 0;
    if (key.value.empty()) {
        return false;
    }

    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT user_id FROM player_challenges "
        "WHERE challenge_key = $1 "
        "  AND claimed_at IS NULL "
        "  AND expires_at > to_timestamp($2) "
        "FOR UPDATE",
        pqxx::params{key.value, nowUnixSeconds});
    if (rows.empty()) {
        tx.commit();
        return false;
    }

    outUserId = rows[0]["user_id"].as<int64_t>();
    tx.exec(
        "UPDATE player_challenges "
        "SET claimed_at = to_timestamp($2) "
        "WHERE challenge_key = $1",
        pqxx::params{key.value, nowUnixSeconds});
    tx.commit();
    return true;
}

void PostgresPlayerSessionStore::create(int64_t userId,
                                         const PlayerSessionKey& key,
                                         int64_t expiresAtUnix) {
    pqxx::work tx(*connection);
    tx.exec(
        "INSERT INTO player_sessions (session_token, user_id, expires_at) "
        "VALUES ($1, $2, to_timestamp($3))",
        pqxx::params{key.value, userId, expiresAtUnix});
    tx.commit();
}

bool PostgresPlayerSessionStore::extend(const PlayerSessionKey& key,
                                        int64_t nowUnixSeconds,
                                        int64_t newExpiresAtUnix) {
    pqxx::work tx(*connection);
    const pqxx::result result = tx.exec(
        std::string("UPDATE player_sessions "
                    "SET expires_at = to_timestamp($3) ") +
            kActiveSessionPredicate,
        pqxx::params{key.value, nowUnixSeconds, newExpiresAtUnix});
    tx.commit();
    return result.affected_rows() > 0;
}

bool PostgresPlayerSessionStore::isActive(const PlayerSessionKey& key,
                                          int64_t nowUnixSeconds) const {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT 1 FROM player_sessions " + std::string(kActiveSessionPredicate),
        pqxx::params{key.value, nowUnixSeconds});
    tx.commit();
    return !rows.empty();
}

}

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include <pqxx/pqxx>

#include "PostgresChallengeServerBootstrap.h"

namespace {

const char* kSkipMessage =
    "SEED_IDENTITY_DB_URL not set; skipping Postgres challenge bootstrap test\n";

void restoreEnv(const char* var, const char* saved) {
    if (saved == nullptr) {
        unsetenv(var);
        return;
    }
    setenv(var, saved, 1);
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
                     int64_t expiresAtUnix) {
    clearChallenge(connection, key);
    pqxx::work tx(connection);
    tx.exec(
        "INSERT INTO player_challenges (challenge_key, user_id, expires_at) "
        "VALUES ($1, $2, to_timestamp($3))",
        pqxx::params{key, userId, expiresAtUnix});
    tx.commit();
}

} // namespace

namespace postgres_challenge_server_bootstrap_tests {

void fails_when_identity_db_url_is_unset() {
    const char* savedAuth = std::getenv("SEED_CHALLENGE_AUTH");
    const char* savedDb = std::getenv("SEED_IDENTITY_DB_URL");
    setenv("SEED_CHALLENGE_AUTH", "1", 1);
    unsetenv("SEED_IDENTITY_DB_URL");

    session::SessionRegistry registry;
    server::PostgresChallengeProductionState state;
    std::string error;
    const std::unique_ptr<server::ServerCommandDispatcher> dispatcher =
        server::createPostgresChallengeProductionDispatcher(registry, state,
                                                            error);
    assert(dispatcher.get() == nullptr);
    assert(!error.empty());

    restoreEnv("SEED_CHALLENGE_AUTH", savedAuth);
    restoreEnv("SEED_IDENTITY_DB_URL", savedDb);
}

void creates_challenge_dispatcher_when_env_and_db_are_set() {
    const char* savedAuth = std::getenv("SEED_CHALLENGE_AUTH");
    const char* savedDb = std::getenv("SEED_IDENTITY_DB_URL");
    if (savedDb == nullptr || savedDb[0] == '\0') {
        std::cerr << kSkipMessage;
        return;
    }

    setenv("SEED_CHALLENGE_AUTH", "1", 1);
    setenv("SEED_IDENTITY_DB_URL", savedDb, 1);

    pqxx::connection connection(savedDb);
    const int64_t userId =
        ensureTestUser(connection, "postgres-bootstrap-user");
    const int64_t now = 1700000000;
    const std::string challengeKey = "postgres-bootstrap-challenge";
    insertChallenge(connection, challengeKey, userId, now + 120);

    session::SessionRegistry registry;
    server::PostgresChallengeProductionState state;
    std::string error;
    const std::unique_ptr<server::ServerCommandDispatcher> dispatcher =
        server::createPostgresChallengeProductionDispatcher(registry, state,
                                                            error);
    assert(dispatcher.get() != nullptr);
    assert(error.empty());
    assert(state.sessionStore.get() != nullptr);
    assert(state.authService.get() != nullptr);
    assert(state.gameplay.get() != nullptr);

    const network::NetworkCommand nicknameLogin = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        "player"};
    const server::CommandDispatchResult nicknameResult =
        dispatcher->dispatch(nicknameLogin);
    assert(!nicknameResult.accepted);
    assert(nicknameResult.error == "invalid_challenge");

    const network::NetworkCommand challengeLogin = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        challengeKey};
    const server::CommandDispatchResult challengeResult =
        dispatcher->dispatch(challengeLogin);
    assert(challengeResult.accepted);
    assert(challengeResult.session.authenticated);
    assert(!challengeResult.playerSessionKey.value.empty());

    clearChallenge(connection, challengeKey);
    restoreEnv("SEED_CHALLENGE_AUTH", savedAuth);
    restoreEnv("SEED_IDENTITY_DB_URL", savedDb);
}

} // namespace postgres_challenge_server_bootstrap_tests

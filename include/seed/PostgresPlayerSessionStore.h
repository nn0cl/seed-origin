#ifndef SEED_POSTGRES_PLAYER_SESSION_STORE_H
#define SEED_POSTGRES_PLAYER_SESSION_STORE_H

#include <memory>
#include <string>

#include "ChallengeSessionPorts.h"

namespace pqxx {
class connection;
}

namespace server {

// Postgres-backed PlayerChallengeClaimPort + PlayerSessionStorePort for
// LISS-0147. SQL and transactions stay inside this adapter.
class PostgresPlayerSessionStore final : public PlayerChallengeClaimPort,
                                         public PlayerSessionStorePort {
public:
    explicit PostgresPlayerSessionStore(const std::string& connectionString);
    ~PostgresPlayerSessionStore() override;

    // Reads SEED_IDENTITY_DB_URL. Returns nullptr when unset so callers can
    // skip integration tests or fail fast during production wiring.
    static std::unique_ptr<PostgresPlayerSessionStore> fromEnvironment();

    PostgresPlayerSessionStore(const PostgresPlayerSessionStore&) = delete;
    PostgresPlayerSessionStore& operator=(const PostgresPlayerSessionStore&) =
        delete;

    bool claim(const ChallengeKey& key,
               int64_t nowUnixSeconds,
               int64_t& outUserId) override;
    void create(int64_t userId,
                const PlayerSessionKey& key,
                int64_t expiresAtUnix) override;
    bool extend(const PlayerSessionKey& key,
                int64_t nowUnixSeconds,
                int64_t newExpiresAtUnix) override;
    bool isActive(const PlayerSessionKey& key,
                  int64_t nowUnixSeconds) const override;

private:
    std::unique_ptr<pqxx::connection> connection;
};

}

#endif

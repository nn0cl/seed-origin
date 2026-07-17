#ifndef SEED_POSTGRES_IDENTITY_ALIAS_STORE_H
#define SEED_POSTGRES_IDENTITY_ALIAS_STORE_H

#include <memory>
#include <string>

#include "IdentityAliasStore.h"

namespace pqxx {
class connection;
}

namespace session {

class PostgresIdentityAliasStore final : public IdentityAliasStore {
public:
    explicit PostgresIdentityAliasStore(const std::string& connectionString);
    ~PostgresIdentityAliasStore() override;

    // Reads the libpq connection string from the SEED_IDENTITY_DB_URL
    // environment variable. Returns nullptr when the variable is unset so
    // callers can fall back to InMemoryIdentityAliasStore instead of
    // embedding credentials in source or config files.
    static std::unique_ptr<PostgresIdentityAliasStore> fromEnvironment();

    PostgresIdentityAliasStore(const PostgresIdentityAliasStore&) = delete;
    PostgresIdentityAliasStore& operator=(const PostgresIdentityAliasStore&) = delete;

    bool find(const std::string& canonicalClaimedId,
              IdentityAliasRecord& record) const override;
    bool insert(const IdentityAliasRecord& record) override;
    bool touch(const std::string& canonicalClaimedId,
               uint64_t lastUsedTick) override;
    bool reviewAlias(const std::string& canonicalClaimedId,
                     AliasReviewStatus status,
                     float confidence) override;
    bool erase(const std::string& canonicalClaimedId) override;
    std::vector<IdentityAliasRecord> exportRecords() const override;

private:
    std::unique_ptr<pqxx::connection> connection;
};

}

#endif

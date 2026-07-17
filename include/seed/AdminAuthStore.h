#ifndef SEED_ADMIN_AUTH_STORE_H
#define SEED_ADMIN_AUTH_STORE_H

#include <cstdint>
#include <memory>
#include <string>

namespace pqxx {
class connection;
}

namespace admin {

class AdminAuthStore {
public:
    explicit AdminAuthStore(const std::string& connectionString);
    ~AdminAuthStore();

    AdminAuthStore(const AdminAuthStore&) = delete;
    AdminAuthStore& operator=(const AdminAuthStore&) = delete;

    // Reads the libpq connection string from SEED_IDENTITY_DB_URL (the same
    // database as PostgresIdentityAliasStore; admin_users is a table in the
    // same seed_identity database per ADR 0017 decision 1).
    static std::unique_ptr<AdminAuthStore> fromEnvironment();

    // Verifies username/password against admin_users.password_hash using
    // PostgreSQL's pgcrypto crypt(). Returns false without distinguishing
    // "unknown user" from "wrong password" to avoid username enumeration.
    bool verifyCredentials(const std::string& username, const std::string& password,
                           int64_t& adminUserId) const;

private:
    std::unique_ptr<pqxx::connection> connection;
};

}

#endif

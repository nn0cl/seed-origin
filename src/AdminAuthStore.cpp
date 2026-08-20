#include "AdminAuthStore.h"

#include <cstdlib>

#include <pqxx/pqxx>

namespace admin {

AdminAuthStore::AdminAuthStore(const std::string& connectionString)
    : connection(std::make_unique<pqxx::connection>(connectionString)) {}

AdminAuthStore::~AdminAuthStore() = default;

std::unique_ptr<AdminAuthStore> AdminAuthStore::fromEnvironment() {
    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    if (connectionString == nullptr || connectionString[0] == '\0') return nullptr;
    return std::make_unique<AdminAuthStore>(std::string(connectionString));
}

bool AdminAuthStore::verifyCredentials(const std::string& username, const std::string& password,
                                       int64_t& adminUserId) const {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT id FROM admin_users "
        "WHERE username = $1 AND password_hash = crypt($2, password_hash)",
        pqxx::params{username, password});
    tx.commit();
    if (rows.empty()) return false;
    adminUserId = rows[0]["id"].as<int64_t>();
    return true;
}

}

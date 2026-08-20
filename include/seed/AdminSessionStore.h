#ifndef SEED_ADMIN_SESSION_STORE_H
#define SEED_ADMIN_SESSION_STORE_H

#include <cstdint>
#include <functional>
#include <map>
#include <string>

namespace admin {

struct AdminSession {
    int64_t adminUserId;
    uint64_t expiresAtEpochSeconds;
};

class AdminSessionStore {
public:
    using TokenGenerator = std::function<std::string()>;

    // Default constructor generates tokens from /dev/urandom.
    AdminSessionStore();
    explicit AdminSessionStore(TokenGenerator generator);

    std::string createSession(int64_t adminUserId, uint64_t nowEpochSeconds,
                              uint64_t ttlSeconds);
    bool validate(const std::string& token, uint64_t nowEpochSeconds,
                 int64_t& adminUserId) const;
    bool invalidate(const std::string& token);

private:
    TokenGenerator generator;
    std::map<std::string, AdminSession> sessions;
};

}

#endif

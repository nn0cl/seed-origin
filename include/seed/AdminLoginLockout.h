#ifndef SEED_ADMIN_LOGIN_LOCKOUT_H
#define SEED_ADMIN_LOGIN_LOCKOUT_H

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>

namespace admin {

// Brute-force guard for /login: after maxAttempts consecutive failures for a
// username, further attempts are rejected until lockoutSeconds have passed.
// A successful login clears the failure count.
class AdminLoginLockout {
public:
    AdminLoginLockout(std::size_t maxAttempts, uint64_t lockoutSeconds);

    bool isLocked(const std::string& username, uint64_t nowEpochSeconds) const;
    void recordFailure(const std::string& username, uint64_t nowEpochSeconds);
    void recordSuccess(const std::string& username);

private:
    struct LockoutState {
        std::size_t consecutiveFailures;
        uint64_t lockedUntilEpochSeconds;
    };

    std::size_t maxAttempts;
    uint64_t lockoutSeconds;
    std::map<std::string, LockoutState> states;
};

}

#endif

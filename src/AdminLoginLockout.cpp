#include "AdminLoginLockout.h"

namespace admin {

AdminLoginLockout::AdminLoginLockout(std::size_t maxAttempts, uint64_t lockoutSeconds)
    : maxAttempts(maxAttempts), lockoutSeconds(lockoutSeconds), states() {}

bool AdminLoginLockout::isLocked(const std::string& username, uint64_t nowEpochSeconds) const {
    const std::map<std::string, LockoutState>::const_iterator found = states.find(username);
    if (found == states.end()) return false;
    return found->second.consecutiveFailures >= maxAttempts &&
           nowEpochSeconds < found->second.lockedUntilEpochSeconds;
}

void AdminLoginLockout::recordFailure(const std::string& username, uint64_t nowEpochSeconds) {
    LockoutState& state = states[username];
    ++state.consecutiveFailures;
    if (state.consecutiveFailures >= maxAttempts) {
        state.lockedUntilEpochSeconds = nowEpochSeconds + lockoutSeconds;
    }
}

void AdminLoginLockout::recordSuccess(const std::string& username) {
    states.erase(username);
}

}

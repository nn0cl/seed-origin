#include "AdminSessionStore.h"

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <sstream>
#include <iomanip>

namespace admin {

namespace {
std::string generateTokenFromUrandom() {
    std::array<unsigned char, 32> bytes{};
    const int fd = ::open("/dev/urandom", O_RDONLY);
    if (fd < 0) return std::string();
    std::size_t total = 0;
    while (total < bytes.size()) {
        const ssize_t got = ::read(fd, bytes.data() + total, bytes.size() - total);
        if (got <= 0) {
            ::close(fd);
            return std::string();
        }
        total += static_cast<std::size_t>(got);
    }
    ::close(fd);

    std::ostringstream hex;
    hex << std::hex << std::setfill('0');
    for (unsigned char byte : bytes) {
        hex << std::setw(2) << static_cast<int>(byte);
    }
    return hex.str();
}
}

AdminSessionStore::AdminSessionStore()
    : generator(&generateTokenFromUrandom), sessions() {}

AdminSessionStore::AdminSessionStore(TokenGenerator generator)
    : generator(std::move(generator)), sessions() {}

std::string AdminSessionStore::createSession(int64_t adminUserId, uint64_t nowEpochSeconds,
                                             uint64_t ttlSeconds) {
    const std::string token = generator();
    if (token.empty()) return token;
    sessions[token] = AdminSession{adminUserId, nowEpochSeconds + ttlSeconds};
    return token;
}

bool AdminSessionStore::validate(const std::string& token, uint64_t nowEpochSeconds,
                                 int64_t& adminUserId) const {
    if (token.empty()) return false;
    const std::map<std::string, AdminSession>::const_iterator found = sessions.find(token);
    if (found == sessions.end()) return false;
    if (found->second.expiresAtEpochSeconds <= nowEpochSeconds) return false;
    adminUserId = found->second.adminUserId;
    return true;
}

bool AdminSessionStore::invalidate(const std::string& token) {
    return sessions.erase(token) > 0;
}

}

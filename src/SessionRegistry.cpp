#include "SessionRegistry.h"

#include <ctype.h>
#include <limits>

namespace session {

SessionRegistry::SessionRegistry()
    : nextInternalId(1), nextAliasId(1) {}

bool SessionRegistry::isValidClaimedId(const std::string& claimedId) {
    if (claimedId.empty() || claimedId.size() > 64) return false;
    for (std::string::const_iterator it = claimedId.begin(); it != claimedId.end(); ++it) {
        if (!isalnum(static_cast<unsigned char>(*it)) && *it != '_' &&
            *it != '-' && *it != '.') {
            return false;
        }
    }
    return true;
}

std::string SessionRegistry::canonicalClaimedId(const std::string& claimedId) {
    std::string canonical;
    canonical.reserve(claimedId.size());
    for (std::string::const_iterator it = claimedId.begin();
         it != claimedId.end(); ++it) {
        canonical.push_back(static_cast<char>(tolower(
            static_cast<unsigned char>(*it))));
    }
    return canonical;
}

SessionInfo SessionRegistry::login(const std::string& claimedId) {
    SessionInfo result = {0, 0, std::string(), false};
    if (nextInternalId == std::numeric_limits<int64_t>::max()) return result;

    result.internalId = nextInternalId++;
    activeSessions.insert(result.internalId);
    if (!isValidClaimedId(claimedId)) return result;

    const std::string canonicalId = canonicalClaimedId(claimedId);
    std::map<std::string, int64_t>::const_iterator found = aliases.find(canonicalId);
    if (found == aliases.end()) {
        if (nextAliasId == std::numeric_limits<int64_t>::max()) return result;
        aliases[canonicalId] = nextAliasId;
        result.aliasId = nextAliasId++;
    } else {
        result.aliasId = found->second;
    }
    result.claimedId = claimedId;
    return result;
}

bool SessionRegistry::logout(int64_t internalId) {
    return activeSessions.erase(internalId) > 0;
}

bool SessionRegistry::isActive(int64_t internalId) const {
    return activeSessions.find(internalId) != activeSessions.end();
}

}

#ifndef SEED_SESSION_REGISTRY_H
#define SEED_SESSION_REGISTRY_H

#include <stdint.h>
#include <map>
#include <set>
#include <string>

namespace session {

struct SessionInfo {
    int64_t internalId;
    int64_t aliasId;
    std::string claimedId;
    bool authenticated;
};

class SessionRegistry {
public:
    SessionRegistry();

    SessionInfo login(const std::string& claimedId);
    bool logout(int64_t internalId);
    bool isActive(int64_t internalId) const;

private:
    static bool isValidClaimedId(const std::string& claimedId);

    int64_t nextInternalId;
    int64_t nextAliasId;
    std::map<std::string, int64_t> aliases;
    std::set<int64_t> activeSessions;
};

}

#endif

#ifndef SEED_SESSION_REGISTRY_H
#define SEED_SESSION_REGISTRY_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "IdentityAliasStore.h"

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
    explicit SessionRegistry(IdentityAliasStore& aliasStore);

    SessionInfo login(const std::string& claimedId);
    SessionInfo login(const std::string& claimedId, uint64_t worldTick);
    bool logout(int64_t internalId);
    bool isActive(int64_t internalId) const;
    std::vector<IdentityAliasRecord> exportAliasRecords() const;
    bool forgetClaimedId(const std::string& claimedId);

private:
    static bool isValidClaimedId(const std::string& claimedId);
    static std::string canonicalClaimedId(const std::string& claimedId);

    int64_t nextInternalId;
    int64_t nextAliasId;
    std::set<int64_t> activeSessions;
    InMemoryIdentityAliasStore defaultAliasStore;
    IdentityAliasStore* aliasStore;
};

}

#endif

#include "SessionRegistry.h"

#include <ctype.h>
#include <limits>

namespace session {

namespace {
int64_t nextAliasIdFor(const IdentityAliasStore& store) {
    int64_t nextId = 1;
    const std::vector<IdentityAliasRecord> records = store.exportRecords();
    for (std::vector<IdentityAliasRecord>::const_iterator it = records.begin();
         it != records.end(); ++it) {
        if (it->aliasId >= nextId && it->aliasId < std::numeric_limits<int64_t>::max()) {
            nextId = it->aliasId + 1;
        }
    }
    return nextId;
}
}

SessionRegistry::SessionRegistry()
    : nextInternalId(1), nextAliasId(1), activeSessions(), defaultAliasStore(),
      aliasStore(&defaultAliasStore) {}

SessionRegistry::SessionRegistry(IdentityAliasStore& aliasStore)
    : nextInternalId(1), nextAliasId(nextAliasIdFor(aliasStore)), activeSessions(), defaultAliasStore(),
      aliasStore(&aliasStore) {}

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
    return login(claimedId, 0);
}

SessionInfo SessionRegistry::login(const std::string& claimedId, uint64_t worldTick) {
    SessionInfo result = {0, 0, std::string(), false};
    if (nextInternalId == std::numeric_limits<int64_t>::max()) return result;

    if (isValidClaimedId(claimedId)) {
        const std::string canonicalId = canonicalClaimedId(claimedId);
        IdentityAliasRecord record = {};
        if (aliasStore->find(canonicalId, record)) {
            result.aliasId = record.aliasId;
            if (!aliasStore->touch(canonicalId, worldTick)) return result;
        } else {
            if (nextAliasId == std::numeric_limits<int64_t>::max()) return result;
            record = {nextAliasId, canonicalId, worldTick, worldTick, 1.0f,
                      AliasReviewStatus::Unreviewed};
            if (!aliasStore->insert(record)) return result;
            result.aliasId = nextAliasId++;
        }
        result.claimedId = claimedId;
    }
    result.internalId = nextInternalId++;
    activeSessions.insert(result.internalId);
    return result;
}

bool SessionRegistry::logout(int64_t internalId) {
    return activeSessions.erase(internalId) > 0;
}

bool SessionRegistry::isActive(int64_t internalId) const {
    return activeSessions.find(internalId) != activeSessions.end();
}

std::vector<IdentityAliasRecord> SessionRegistry::exportAliasRecords() const {
    return aliasStore->exportRecords();
}

bool SessionRegistry::forgetClaimedId(const std::string& claimedId) {
    if (!isValidClaimedId(claimedId)) return false;
    return aliasStore->erase(canonicalClaimedId(claimedId));
}

}

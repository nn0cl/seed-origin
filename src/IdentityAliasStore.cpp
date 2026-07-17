#include "IdentityAliasStore.h"

namespace session {

bool InMemoryIdentityAliasStore::find(const std::string& canonicalClaimedId,
                                      IdentityAliasRecord& record) const {
    const std::map<std::string, IdentityAliasRecord>::const_iterator found =
        records.find(canonicalClaimedId);
    if (found == records.end()) return false;
    record = found->second;
    return true;
}

bool InMemoryIdentityAliasStore::insert(const IdentityAliasRecord& record) {
    if (record.aliasId <= 0 || record.canonicalClaimedId.empty() ||
        record.confidence < 0.0f || record.confidence > 1.0f ||
        records.find(record.canonicalClaimedId) != records.end()) {
        return false;
    }
    records.emplace(record.canonicalClaimedId, record);
    return true;
}

bool InMemoryIdentityAliasStore::touch(const std::string& canonicalClaimedId,
                                       uint64_t lastUsedTick) {
    std::map<std::string, IdentityAliasRecord>::iterator found =
        records.find(canonicalClaimedId);
    if (found == records.end()) return false;
    if (lastUsedTick > found->second.lastUsedTick) {
        found->second.lastUsedTick = lastUsedTick;
    }
    return true;
}

bool InMemoryIdentityAliasStore::erase(const std::string& canonicalClaimedId) {
    return records.erase(canonicalClaimedId) > 0;
}

std::vector<IdentityAliasRecord> InMemoryIdentityAliasStore::exportRecords() const {
    std::vector<IdentityAliasRecord> result;
    result.reserve(records.size());
    for (std::map<std::string, IdentityAliasRecord>::const_iterator it = records.begin();
         it != records.end(); ++it) {
        result.push_back(it->second);
    }
    return result;
}

}

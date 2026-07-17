#ifndef SEED_IDENTITY_ALIAS_STORE_H
#define SEED_IDENTITY_ALIAS_STORE_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace session {

enum class AliasReviewStatus {
    Unreviewed,
    HumanConfirmed,
    HumanRejected
};

struct IdentityAliasRecord {
    int64_t aliasId;
    std::string canonicalClaimedId;
    uint64_t createdTick;
    uint64_t lastUsedTick;
    float confidence;
    AliasReviewStatus reviewStatus;
};

class IdentityAliasStore {
public:
    virtual ~IdentityAliasStore() = default;
    virtual bool find(const std::string& canonicalClaimedId,
                      IdentityAliasRecord& record) const = 0;
    virtual bool insert(const IdentityAliasRecord& record) = 0;
    virtual bool touch(const std::string& canonicalClaimedId,
                       uint64_t lastUsedTick) = 0;
    virtual bool erase(const std::string& canonicalClaimedId) = 0;
    virtual std::vector<IdentityAliasRecord> exportRecords() const = 0;
};

class InMemoryIdentityAliasStore final : public IdentityAliasStore {
public:
    bool find(const std::string& canonicalClaimedId,
              IdentityAliasRecord& record) const override;
    bool insert(const IdentityAliasRecord& record) override;
    bool touch(const std::string& canonicalClaimedId,
               uint64_t lastUsedTick) override;
    bool erase(const std::string& canonicalClaimedId) override;
    std::vector<IdentityAliasRecord> exportRecords() const override;

private:
    std::map<std::string, IdentityAliasRecord> records;
};

}

#endif

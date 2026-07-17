#include "PostgresIdentityAliasStore.h"

#include <cstdlib>

#include <pqxx/pqxx>

namespace session {

namespace {
AliasReviewStatus reviewStatusFromInt(int value) {
    switch (value) {
        case 1: return AliasReviewStatus::HumanConfirmed;
        case 2: return AliasReviewStatus::HumanRejected;
        default: return AliasReviewStatus::Unreviewed;
    }
}

int reviewStatusToInt(AliasReviewStatus status) {
    switch (status) {
        case AliasReviewStatus::HumanConfirmed: return 1;
        case AliasReviewStatus::HumanRejected: return 2;
        default: return 0;
    }
}
}

PostgresIdentityAliasStore::PostgresIdentityAliasStore(const std::string& connectionString)
    : connection(std::make_unique<pqxx::connection>(connectionString)) {}

PostgresIdentityAliasStore::~PostgresIdentityAliasStore() = default;

std::unique_ptr<PostgresIdentityAliasStore> PostgresIdentityAliasStore::fromEnvironment() {
    const char* connectionString = std::getenv("SEED_IDENTITY_DB_URL");
    if (connectionString == nullptr || connectionString[0] == '\0') return nullptr;
    return std::make_unique<PostgresIdentityAliasStore>(std::string(connectionString));
}

bool PostgresIdentityAliasStore::find(const std::string& canonicalClaimedId,
                                      IdentityAliasRecord& record) const {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT alias_id, canonical_claimed_id, created_tick, last_used_tick, "
        "confidence, review_status FROM identity_aliases "
        "WHERE canonical_claimed_id = $1",
        pqxx::params{canonicalClaimedId});
    tx.commit();
    if (rows.empty()) return false;
    const auto row = rows[0];
    record.aliasId = row["alias_id"].as<int64_t>();
    record.canonicalClaimedId = row["canonical_claimed_id"].as<std::string>();
    record.createdTick = row["created_tick"].as<uint64_t>();
    record.lastUsedTick = row["last_used_tick"].as<uint64_t>();
    record.confidence = row["confidence"].as<float>();
    record.reviewStatus = reviewStatusFromInt(row["review_status"].as<int>());
    return true;
}

bool PostgresIdentityAliasStore::insert(const IdentityAliasRecord& record) {
    if (record.aliasId <= 0 || record.canonicalClaimedId.empty() ||
        record.confidence < 0.0f || record.confidence > 1.0f) {
        return false;
    }
    pqxx::work tx(*connection);
    try {
        tx.exec(
            "INSERT INTO identity_aliases "
            "(alias_id, canonical_claimed_id, created_tick, last_used_tick, "
            "confidence, review_status) VALUES ($1, $2, $3, $4, $5, $6)",
            pqxx::params{record.aliasId, record.canonicalClaimedId,
                         static_cast<int64_t>(record.createdTick),
                         static_cast<int64_t>(record.lastUsedTick),
                         record.confidence,
                         reviewStatusToInt(record.reviewStatus)});
    } catch (const pqxx::unique_violation&) {
        return false;
    }
    tx.commit();
    return true;
}

bool PostgresIdentityAliasStore::touch(const std::string& canonicalClaimedId,
                                       uint64_t lastUsedTick) {
    pqxx::work tx(*connection);
    const pqxx::result result = tx.exec(
        "UPDATE identity_aliases "
        "SET last_used_tick = GREATEST(last_used_tick, $2) "
        "WHERE canonical_claimed_id = $1",
        pqxx::params{canonicalClaimedId, static_cast<int64_t>(lastUsedTick)});
    tx.commit();
    return result.affected_rows() > 0;
}

bool PostgresIdentityAliasStore::reviewAlias(const std::string& canonicalClaimedId,
                                             AliasReviewStatus status,
                                             float confidence) {
    if (confidence < 0.0f || confidence > 1.0f) return false;
    pqxx::work tx(*connection);
    const pqxx::result result = tx.exec(
        "UPDATE identity_aliases SET review_status = $2, confidence = $3 "
        "WHERE canonical_claimed_id = $1",
        pqxx::params{canonicalClaimedId, reviewStatusToInt(status), confidence});
    tx.commit();
    return result.affected_rows() > 0;
}

bool PostgresIdentityAliasStore::erase(const std::string& canonicalClaimedId) {
    pqxx::work tx(*connection);
    const pqxx::result result = tx.exec(
        "DELETE FROM identity_aliases WHERE canonical_claimed_id = $1",
        pqxx::params{canonicalClaimedId});
    tx.commit();
    return result.affected_rows() > 0;
}

std::vector<IdentityAliasRecord> PostgresIdentityAliasStore::exportRecords() const {
    pqxx::work tx(*connection);
    const pqxx::result rows = tx.exec(
        "SELECT alias_id, canonical_claimed_id, created_tick, last_used_tick, "
        "confidence, review_status FROM identity_aliases ORDER BY alias_id");
    tx.commit();
    std::vector<IdentityAliasRecord> result;
    result.reserve(rows.size());
    for (const auto& row : rows) {
        IdentityAliasRecord record;
        record.aliasId = row["alias_id"].as<int64_t>();
        record.canonicalClaimedId = row["canonical_claimed_id"].as<std::string>();
        record.createdTick = row["created_tick"].as<uint64_t>();
        record.lastUsedTick = row["last_used_tick"].as<uint64_t>();
        record.confidence = row["confidence"].as<float>();
        record.reviewStatus = reviewStatusFromInt(row["review_status"].as<int>());
        result.push_back(record);
    }
    return result;
}

}

#include <cassert>

#include "SessionRegistry.h"

namespace identity_alias_store_tests {

void preserves_alias_metadata_and_case_insensitive_reconciliation() {
    session::InMemoryIdentityAliasStore store;
    session::SessionRegistry registry(store);
    const session::SessionInfo first = registry.login("Player-A", 10);
    const session::SessionInfo second = registry.login("player-a", 20);
    assert(first.internalId != 0 && second.internalId != 0);
    assert(first.aliasId == second.aliasId);
    const std::vector<session::IdentityAliasRecord> records =
        registry.exportAliasRecords();
    assert(records.size() == 1);
    assert(records[0].createdTick == 10 && records[0].lastUsedTick == 20);
    assert(records[0].reviewStatus == session::AliasReviewStatus::Unreviewed);
}

void supports_explicit_claimed_id_deletion_without_affecting_sessions() {
    session::SessionRegistry registry;
    const session::SessionInfo session = registry.login("temporary", 1);
    assert(session.aliasId != 0 && registry.isActive(session.internalId));
    assert(registry.forgetClaimedId("TEMPORARY"));
    assert(registry.exportAliasRecords().empty());
    assert(registry.isActive(session.internalId));
}

void records_human_review_decision_without_affecting_active_sessions() {
    session::SessionRegistry registry;
    const session::SessionInfo session = registry.login("Player-B", 5);
    assert(registry.recordAliasReview("player-b",
                                       session::AliasReviewStatus::HumanConfirmed,
                                       0.5f));
    const std::vector<session::IdentityAliasRecord> records =
        registry.exportAliasRecords();
    assert(records.size() == 1);
    assert(records[0].reviewStatus == session::AliasReviewStatus::HumanConfirmed);
    assert(records[0].confidence == 0.5f);
    assert(registry.isActive(session.internalId));
}

void rejects_review_for_missing_alias_or_invalid_confidence() {
    session::InMemoryIdentityAliasStore store;
    session::SessionRegistry registry(store);
    registry.login("known-id", 1);
    assert(!registry.recordAliasReview("unknown-id",
                                        session::AliasReviewStatus::HumanRejected,
                                        1.0f));
    assert(!registry.recordAliasReview("known-id",
                                        session::AliasReviewStatus::HumanRejected,
                                        1.5f));
    const std::vector<session::IdentityAliasRecord> records = store.exportRecords();
    assert(records.size() == 1);
    assert(records[0].reviewStatus == session::AliasReviewStatus::Unreviewed);
}

} // namespace identity_alias_store_tests

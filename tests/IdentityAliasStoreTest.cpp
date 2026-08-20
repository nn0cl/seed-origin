#include <cassert>

#include "IdentityAliasStore.h"
#include "SessionRegistry.h"

namespace identity_alias_store_tests {

void preserves_alias_metadata_and_case_insensitive_reconciliation() {
    session::InMemoryIdentityAliasStore store;
    session::IdentityAliasRecord first = {
        1, "player-a", 10, 10, 1.0f, session::AliasReviewStatus::Unreviewed};
    assert(store.insert(first));
    assert(store.touch("player-a", 20));

    session::IdentityAliasRecord found = {};
    assert(store.find("player-a", found));
    assert(found.aliasId == 1);
    assert(found.createdTick == 10 && found.lastUsedTick == 20);
    assert(found.reviewStatus == session::AliasReviewStatus::Unreviewed);

    session::SessionRegistry registry(store);
    const std::vector<session::IdentityAliasRecord> records =
        registry.exportAliasRecords();
    assert(records.size() == 1);
    assert(records[0].createdTick == 10 && records[0].lastUsedTick == 20);
}

void supports_explicit_claimed_id_deletion_without_affecting_sessions() {
    session::InMemoryIdentityAliasStore store;
    session::IdentityAliasRecord record = {
        1, "temporary", 1, 1, 1.0f, session::AliasReviewStatus::Unreviewed};
    assert(store.insert(record));
    session::SessionRegistry registry(store);
    const session::SessionInfo session = registry.openAuthenticatedSession(11);
    assert(registry.isActive(session.internalId));
    assert(registry.forgetClaimedId("TEMPORARY"));
    assert(registry.exportAliasRecords().empty());
    assert(registry.isActive(session.internalId));
}

void records_human_review_decision_without_affecting_active_sessions() {
    session::InMemoryIdentityAliasStore store;
    session::IdentityAliasRecord record = {
        1, "player-b", 5, 5, 1.0f, session::AliasReviewStatus::Unreviewed};
    assert(store.insert(record));
    session::SessionRegistry registry(store);
    const session::SessionInfo session = registry.openAuthenticatedSession(12);
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
    session::IdentityAliasRecord record = {
        1, "known-id", 1, 1, 1.0f, session::AliasReviewStatus::Unreviewed};
    assert(store.insert(record));
    session::SessionRegistry registry(store);
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

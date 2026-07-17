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

} // namespace identity_alias_store_tests

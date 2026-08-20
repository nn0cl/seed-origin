#include <cassert>
#include <cstdlib>
#include <iostream>

#include "PostgresIdentityAliasStore.h"
#include "SessionRegistry.h"

namespace postgres_identity_alias_store_tests {

void round_trips_records_through_postgres() {
    std::unique_ptr<session::PostgresIdentityAliasStore> store =
        session::PostgresIdentityAliasStore::fromEnvironment();
    if (!store) {
        std::cerr << "SEED_IDENTITY_DB_URL not set; skipping Postgres adapter test\n";
        return;
    }

    session::SessionRegistry registry(*store);
    const session::SessionInfo first = registry.login("Postgres-Test-User", 1);
    const session::SessionInfo second = registry.login("postgres-test-user", 2);
    assert(first.aliasId != 0 && first.aliasId == second.aliasId);

    assert(registry.recordAliasReview("postgres-test-user",
                                       session::AliasReviewStatus::HumanConfirmed,
                                       0.75f));

    session::IdentityAliasRecord record;
    assert(store->find("postgres-test-user", record));
    assert(record.reviewStatus == session::AliasReviewStatus::HumanConfirmed);
    assert(record.confidence == 0.75f);
    assert(record.lastUsedTick == 2);

    assert(registry.forgetClaimedId("postgres-test-user"));
    assert(!store->find("postgres-test-user", record));
}

} // namespace postgres_identity_alias_store_tests

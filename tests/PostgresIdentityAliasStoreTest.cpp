#include <cassert>
#include <cstdlib>
#include <iostream>

#include "PostgresIdentityAliasStore.h"
#include "SessionRegistry.h"

namespace {

// Adapter integration test per docs/architecture/testing-strategy.md's
// "Adapter Integration Tests" rule and ADR 0016: requires a real PostgreSQL
// instance (see db/docker-compose.yml) reachable via SEED_IDENTITY_DB_URL.
// This is not part of the seed_tests in-memory suite; it only builds/links
// when libpqxx is present (see CMakeLists.txt).
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

} // namespace

int main() {
    round_trips_records_through_postgres();
    return 0;
}

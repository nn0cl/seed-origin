#include <cassert>

#include "SessionRegistry.h"

namespace session_registry_tests {

void assigns_unique_internal_ids_and_stable_aliases() {
    session::SessionRegistry registry;
    const session::SessionInfo first = registry.login("player-a");
    const session::SessionInfo second = registry.login("player-a");
    assert(first.internalId != second.internalId);
    assert(first.aliasId == second.aliasId);
    assert(!first.authenticated && !second.authenticated);
}

void treats_invalid_claims_as_anonymous() {
    session::SessionRegistry registry;
    const session::SessionInfo info = registry.login("../admin");
    assert(info.internalId != 0);
    assert(info.aliasId == 0);
    assert(info.claimedId.empty());
}

void never_reuses_disconnected_ids() {
    session::SessionRegistry registry;
    const session::SessionInfo first = registry.login("");
    assert(registry.isActive(first.internalId));
    assert(registry.logout(first.internalId));
    const session::SessionInfo second = registry.login("");
    assert(first.internalId != second.internalId);
}

} // namespace session_registry_tests

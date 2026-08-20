#include <cassert>

#include "SessionRegistry.h"

namespace session_registry_tests {

void assigns_unique_internal_ids_for_authenticated_sessions() {
    session::SessionRegistry registry;
    const session::SessionInfo first = registry.openAuthenticatedSession(101);
    const session::SessionInfo second = registry.openAuthenticatedSession(101);
    assert(first.internalId != 0);
    assert(second.internalId != 0);
    assert(first.internalId != second.internalId);
    assert(first.authenticated && second.authenticated);
}

void rejects_non_positive_user_ids() {
    session::SessionRegistry registry;
    const session::SessionInfo zero = registry.openAuthenticatedSession(0);
    const session::SessionInfo negative = registry.openAuthenticatedSession(-1);
    assert(zero.internalId == 0);
    assert(!zero.authenticated);
    assert(negative.internalId == 0);
    assert(!negative.authenticated);
}

void never_reuses_disconnected_ids() {
    session::SessionRegistry registry;
    const session::SessionInfo first = registry.openAuthenticatedSession(7);
    assert(registry.isActive(first.internalId));
    assert(registry.logout(first.internalId));
    const session::SessionInfo second = registry.openAuthenticatedSession(7);
    assert(first.internalId != second.internalId);
}

} // namespace session_registry_tests

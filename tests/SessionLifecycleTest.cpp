#include <cassert>

#include "SessionLifecycle.h"

namespace session_lifecycle_tests {

void rejects_duplicate_login_on_one_connection() {
    session::SessionRegistry registry;
    const session::SessionInfo first = registry.login("player-a");
    const session::SessionInfo second = registry.login("player-b");
    server::SessionLifecycle lifecycle;
    std::string error;
    assert(lifecycle.bind(1, first, error));
    assert(!lifecycle.bind(1, second, error));
    assert(lifecycle.sessionId(1) == first.internalId);
    assert(registry.isActive(second.internalId));
}

void logs_out_bound_session_on_disconnect() {
    session::SessionRegistry registry;
    const session::SessionInfo info = registry.login("player-a");
    server::SessionLifecycle lifecycle;
    std::string error;
    assert(lifecycle.bind(1, info, error));
    assert(lifecycle.disconnect(1, registry));
    assert(!registry.isActive(info.internalId));
    assert(!lifecycle.hasSession(1));
    assert(!lifecycle.disconnect(1, registry));
}

void rejects_invalid_binding_without_mutation() {
    server::SessionLifecycle lifecycle;
    const session::SessionInfo empty = {0, 0, std::string(), false};
    std::string error;
    assert(!lifecycle.bind(1, empty, error));
    assert(!lifecycle.hasSession(1));
}

void clears_all_bound_sessions() {
    session::SessionRegistry registry;
    const session::SessionInfo first = registry.login("player-a");
    const session::SessionInfo second = registry.login("player-b");
    server::SessionLifecycle lifecycle;
    std::string error;
    assert(lifecycle.bind(1, first, error));
    assert(lifecycle.bind(2, second, error));
    lifecycle.clear(registry);
    assert(!registry.isActive(first.internalId));
    assert(!registry.isActive(second.internalId));
    assert(!lifecycle.hasSession(1));
    assert(!lifecycle.hasSession(2));
}

} // namespace session_lifecycle_tests

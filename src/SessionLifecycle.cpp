#include "SessionLifecycle.h"

namespace server {

bool SessionLifecycle::bind(uint64_t connectionId,
                            const session::SessionInfo& info,
                            std::string& error) {
    if (connectionId == 0 || info.internalId <= 0) {
        error = "session binding identity is invalid";
        return false;
    }
    if (bindings.find(connectionId) != bindings.end()) {
        error = "connection already has a session";
        return false;
    }
    bindings[connectionId] = info.internalId;
    error.clear();
    return true;
}

bool SessionLifecycle::disconnect(uint64_t connectionId,
                                  session::SessionRegistry& registry) {
    const std::map<uint64_t, int64_t>::iterator found = bindings.find(connectionId);
    if (found == bindings.end()) return false;
    const bool loggedOut = registry.logout(found->second);
    bindings.erase(found);
    return loggedOut;
}

void SessionLifecycle::clear() {
    bindings.clear();
}

void SessionLifecycle::clear(session::SessionRegistry& registry) {
    for (std::map<uint64_t, int64_t>::const_iterator it = bindings.begin();
         it != bindings.end(); ++it) {
        registry.logout(it->second);
    }
    bindings.clear();
}

bool SessionLifecycle::hasSession(uint64_t connectionId) const {
    return bindings.find(connectionId) != bindings.end();
}

int64_t SessionLifecycle::sessionId(uint64_t connectionId) const {
    const std::map<uint64_t, int64_t>::const_iterator found = bindings.find(connectionId);
    return found == bindings.end() ? 0 : found->second;
}

}

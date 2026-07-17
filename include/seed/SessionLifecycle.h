#ifndef SEED_SESSION_LIFECYCLE_H
#define SEED_SESSION_LIFECYCLE_H

#include <cstdint>
#include <map>
#include <string>

#include "SessionRegistry.h"

namespace server {

class SessionLifecycle {
public:
    bool bind(uint64_t connectionId, const session::SessionInfo& info,
              std::string& error);
    bool disconnect(uint64_t connectionId, session::SessionRegistry& registry);
    void clear();
    void clear(session::SessionRegistry& registry);
    bool hasSession(uint64_t connectionId) const;
    int64_t sessionId(uint64_t connectionId) const;

private:
    std::map<uint64_t, int64_t> bindings;
};

}

#endif

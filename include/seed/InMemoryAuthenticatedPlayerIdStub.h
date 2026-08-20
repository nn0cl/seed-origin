#ifndef SEED_IN_MEMORY_AUTHENTICATED_PLAYER_ID_STUB_H
#define SEED_IN_MEMORY_AUTHENTICATED_PLAYER_ID_STUB_H

#include <cstdint>
#include <map>
#include <string>

#include "AuthenticatedPlayerIdPort.h"

namespace server {

// Early stand-in until seed-auth issues PlayerId. Same claimedId maps to the
// same opaque id. Does not copy session.internalId.
class InMemoryAuthenticatedPlayerIdStub : public AuthenticatedPlayerIdPort {
public:
    InMemoryAuthenticatedPlayerIdStub();
    bool resolvePlayerId(int64_t sessionId, const std::string& claimedId,
                         int64_t& playerId) const;

private:
    mutable std::map<std::string, int64_t> byClaimedId;
    mutable int64_t nextPlayerId;
};

}

#endif

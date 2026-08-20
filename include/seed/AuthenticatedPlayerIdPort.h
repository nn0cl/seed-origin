#ifndef SEED_AUTHENTICATED_PLAYER_ID_PORT_H
#define SEED_AUTHENTICATED_PLAYER_ID_PORT_H

#include <cstdint>
#include <string>

namespace server {

// Opaque account key issued by the authentication server. Not a connection id
// and not a Field gameplay id. Real issuance is LISS-0146–0150.
class AuthenticatedPlayerIdPort {
public:
    virtual ~AuthenticatedPlayerIdPort() {}
    virtual bool resolvePlayerId(int64_t sessionId, const std::string& claimedId,
                                 int64_t& playerId) const = 0;
};

}

#endif

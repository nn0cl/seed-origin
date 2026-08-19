#include "InMemoryAuthenticatedPlayerIdStub.h"

namespace server {

InMemoryAuthenticatedPlayerIdStub::InMemoryAuthenticatedPlayerIdStub()
    : byClaimedId(), nextPlayerId(1000001) {}

bool InMemoryAuthenticatedPlayerIdStub::resolvePlayerId(
    int64_t sessionId, const std::string& claimedId, int64_t& playerId) const {
    (void)sessionId;
    if (!claimedId.empty()) {
        const std::map<std::string, int64_t>::const_iterator found =
            byClaimedId.find(claimedId);
        if (found != byClaimedId.end()) {
            playerId = found->second;
            return true;
        }
        playerId = nextPlayerId++;
        byClaimedId[claimedId] = playerId;
        return true;
    }
    playerId = nextPlayerId++;
    return true;
}

}

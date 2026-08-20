#ifndef SEED_RANDOM_SESSION_KEY_ISSUER_H
#define SEED_RANDOM_SESSION_KEY_ISSUER_H

#include "ChallengeSessionPorts.h"

namespace server {

class RandomSessionKeyIssuer : public SessionKeyIssuer {
public:
    PlayerSessionKey issue() override;
};

}

#endif

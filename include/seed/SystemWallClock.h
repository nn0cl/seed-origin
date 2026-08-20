#ifndef SEED_SYSTEM_WALL_CLOCK_H
#define SEED_SYSTEM_WALL_CLOCK_H

#include "ChallengeSessionPorts.h"

namespace server {

class SystemWallClock : public WallClock {
public:
    int64_t nowUnixSeconds() const override;
};

}

#endif

#ifndef SEED_MOVEMENT_SIMULATION_H
#define SEED_MOVEMENT_SIMULATION_H

#include "MovementIntentQueue.h"

namespace server {

[[nodiscard]] bool integrateMovement(float& x, float& y, float& z,
                                     float dx, float dy, float dz);

}

#endif

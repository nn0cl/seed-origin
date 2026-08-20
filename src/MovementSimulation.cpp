#include "MovementSimulation.h"

#include <cmath>

namespace server {

bool integrateMovement(float& x, float& y, float& z, float dx, float dy,
                       float dz) {
    if (!isValidMovementDelta(dx, dy, dz)) return false;
    const float nextX = x + dx;
    const float nextY = y + dy;
    const float nextZ = z + dz;
    if (!std::isfinite(nextX) || !std::isfinite(nextY) || !std::isfinite(nextZ) ||
        std::fabs(nextX) > MAX_WORLD_COORDINATE ||
        std::fabs(nextY) > MAX_WORLD_COORDINATE ||
        std::fabs(nextZ) > MAX_WORLD_COORDINATE) {
        return false;
    }
    x = nextX;
    y = nextY;
    z = nextZ;
    return true;
}

}

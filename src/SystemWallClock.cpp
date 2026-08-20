#include "SystemWallClock.h"

#include <chrono>

namespace server {

int64_t SystemWallClock::nowUnixSeconds() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

}

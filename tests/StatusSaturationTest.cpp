#include <cassert>
#include <limits>

#include "Status.h"

namespace status_saturation_tests {

void saturates_positive_values() {
    Status status(std::numeric_limits<long>::max() - 1, 0);
    assert(status.gainHp(2));
    assert(status.getHp() == std::numeric_limits<long>::max());
}

void saturates_negative_values() {
    Status status(1, 1);
    assert(status.gainHp(std::numeric_limits<long>::min()));
    assert(status.gainMp(-2));
    assert(status.getHp() == 0);
    assert(status.getMp() == 0);
}

} // namespace status_saturation_tests

#include "EnvironmentEther.h"

#include <algorithm>
#include <cmath>

namespace world {

EnvironmentEther::EnvironmentEther() : values{0.0f, 0.0f, 0.0f, 0.0f} {}

bool EnvironmentEther::attributeForElement(const std::string& element,
                                           EtherAttribute& attribute) const {
    if (element == "fire") attribute = EtherAttribute::Fire;
    else if (element == "water") attribute = EtherAttribute::Water;
    else if (element == "earth") attribute = EtherAttribute::Earth;
    else if (element == "air") attribute = EtherAttribute::Air;
    else return false;
    return true;
}

bool EnvironmentEther::isKnownElement(const std::string& element) const {
    EtherAttribute attribute = EtherAttribute::Fire;
    return attributeForElement(element, attribute);
}

float EnvironmentEther::value(EtherAttribute attribute) const {
    return values[static_cast<size_t>(attribute)];
}

float EnvironmentEther::conductivity(EtherAttribute attribute) const {
    const float magnitude = std::fabs(value(attribute));
    return 1.0f + std::min(magnitude / 100.0f, 1.0f);
}

bool EnvironmentEther::add(EtherAttribute attribute, float amount) {
    if (!std::isfinite(amount)) return false;
    float& current = values[static_cast<size_t>(attribute)];
    current = std::max(-MAX_ETHER_VALUE,
                       std::min(MAX_ETHER_VALUE, current + amount));
    return true;
}

bool EnvironmentEther::resolveSpell(const std::string& element, float basePower,
                                    float& effectivePower, std::string& error) {
    EtherAttribute attribute = EtherAttribute::Fire;
    if (!attributeForElement(element, attribute)) {
        error = "spell element is unknown";
        return false;
    }
    if (!std::isfinite(basePower) || basePower <= 0.0f) {
        error = "spell power is invalid";
        return false;
    }
    effectivePower = basePower * conductivity(attribute);
    if (!std::isfinite(effectivePower)) {
        error = "spell effective power is invalid";
        return false;
    }
    if (!add(attribute, basePower * 0.1f)) {
        error = "spell ether influence is invalid";
        return false;
    }
    error.clear();
    return true;
}

void EnvironmentEther::decay() {
    for (size_t i = 0; i < ETHER_ATTRIBUTE_COUNT; ++i) values[i] *= ETHER_DECAY_FACTOR;
}

}

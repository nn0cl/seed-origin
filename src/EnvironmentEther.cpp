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

float EnvironmentEther::instability() const {
    float totalMagnitude = 0.0f;
    for (size_t i = 0; i < ETHER_ATTRIBUTE_COUNT; ++i) {
        totalMagnitude += std::fabs(values[i]);
    }
    return std::min(totalMagnitude / 200.0f, 10.0f);
}

float EnvironmentEther::adverseEffectSeverity() const {
    return std::max(0.0f, instability() - 1.0f);
}

bool EnvironmentEther::hasAdverseEffect() const {
    return adverseEffectSeverity() > 0.0f;
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
    const float environmentalInfluence = basePower * 0.1f;
    if (!add(attribute, environmentalInfluence)) {
        error = "spell ether influence is invalid";
        return false;
    }
    EtherAttribute opposing = EtherAttribute::Fire;
    if (attribute == EtherAttribute::Fire) opposing = EtherAttribute::Water;
    else if (attribute == EtherAttribute::Water) opposing = EtherAttribute::Fire;
    else if (attribute == EtherAttribute::Earth) opposing = EtherAttribute::Air;
    else opposing = EtherAttribute::Earth;
    if (!add(opposing, -environmentalInfluence * 0.25f)) {
        error = "spell ether interaction is invalid";
        return false;
    }
    error.clear();
    return true;
}

void EnvironmentEther::decay() {
    for (size_t i = 0; i < ETHER_ATTRIBUTE_COUNT; ++i) values[i] *= ETHER_DECAY_FACTOR;
}

}

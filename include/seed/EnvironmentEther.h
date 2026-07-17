#ifndef SEED_ENVIRONMENT_ETHER_H
#define SEED_ENVIRONMENT_ETHER_H

#include <stddef.h>
#include <string>

namespace world {

enum class EtherAttribute {
    Fire = 0,
    Water = 1,
    Earth = 2,
    Air = 3
};

static const size_t ETHER_ATTRIBUTE_COUNT = 4;
static const float MAX_ETHER_VALUE = 100000.0f;
static const float ETHER_DECAY_FACTOR = 0.98f;

class EnvironmentEther {
public:
    EnvironmentEther();

    bool attributeForElement(const std::string& element,
                             EtherAttribute& attribute) const;
    bool isKnownElement(const std::string& element) const;
    float value(EtherAttribute attribute) const;
    float conductivity(EtherAttribute attribute) const;
    float instability() const;
    float adverseEffectSeverity() const;
    bool hasAdverseEffect() const;
    bool add(EtherAttribute attribute, float amount);
    bool resolveSpell(const std::string& element, float basePower,
                      float& effectivePower, std::string& error);
    void decay();

private:
    float values[ETHER_ATTRIBUTE_COUNT];
};

}

#endif

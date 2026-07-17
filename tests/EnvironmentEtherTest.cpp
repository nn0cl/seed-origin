#include <cassert>

#include "EnvironmentEther.h"

namespace environment_ether_tests {

void increases_conductivity_with_ether_magnitude() {
    world::EnvironmentEther ether;
    assert(ether.conductivity(world::EtherAttribute::Fire) == 1.0f);
    assert(ether.add(world::EtherAttribute::Fire, 50.0f));
    assert(ether.conductivity(world::EtherAttribute::Fire) > 1.0f);
    assert(ether.add(world::EtherAttribute::Fire, -200.0f));
    assert(ether.conductivity(world::EtherAttribute::Fire) > 1.0f);
}

void resolves_spell_and_decays_environment() {
    world::EnvironmentEther ether;
    float effectivePower = 0.0f;
    std::string error;
    assert(ether.resolveSpell("fire", 100.0f, effectivePower, error));
    assert(effectivePower == 100.0f);
    assert(ether.value(world::EtherAttribute::Fire) == 10.0f);
    ether.decay();
    assert(ether.value(world::EtherAttribute::Fire) < 10.0f);
}

void rejects_unknown_element() {
    world::EnvironmentEther ether;
    float effectivePower = 0.0f;
    std::string error;
    assert(!ether.resolveSpell("void", 100.0f, effectivePower, error));
}

} // namespace environment_ether_tests

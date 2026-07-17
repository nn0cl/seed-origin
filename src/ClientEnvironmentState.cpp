#include "ClientEnvironmentState.h"

namespace client {

ClientEnvironmentState::ClientEnvironmentState()
    : state{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0} {}

const EnvironmentState& ClientEnvironmentState::value() const {
    return state;
}

void ClientEnvironmentState::replace(const EnvironmentState& next) {
    state = next;
}

}

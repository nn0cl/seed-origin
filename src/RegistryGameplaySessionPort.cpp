#include "RegistryGameplaySessionPort.h"

namespace server {

RegistryGameplaySessionPort::RegistryGameplaySessionPort(
    session::SessionRegistry& registry)
    : registry(registry) {}

session::SessionInfo RegistryGameplaySessionPort::openAuthenticated(
    int64_t userId) {
    return registry.openAuthenticatedSession(userId);
}

}

#include "PostgresProductionBootstrapBridge.h"

#include <cstdlib>

#include "ChallengeProductionBootstrapErrors.h"
#include "SessionRegistry.h"

namespace server {

__attribute__((weak)) std::unique_ptr<ServerCommandDispatcher>
postgresProductionBootstrap(session::SessionRegistry& registry,
                            std::string& error,
                            void* ownedState) {
    (void)registry;
    (void)ownedState;

    const char* dbUrl = std::getenv("SEED_IDENTITY_DB_URL");
    if (dbUrl == nullptr || dbUrl[0] == '\0') {
        error = kMissingIdentityDbUrlError;
        return nullptr;
    }

    error = kPostgresBootstrapUnavailableError;
    return nullptr;
}

}

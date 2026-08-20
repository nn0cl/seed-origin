#ifndef SEED_POSTGRES_PRODUCTION_BOOTSTRAP_BRIDGE_H
#define SEED_POSTGRES_PRODUCTION_BOOTSTRAP_BRIDGE_H

#include <memory>
#include <string>

#include "ServerCommandDispatcher.h"

namespace session {
class SessionRegistry;
}

namespace server {

// Live Postgres challenge bootstrap for seed_server (LISS-0147).
// Implemented in seed_postgres when libpqxx is linked; seed_core provides a
// weak stub so unit tests stay libpqxx-free. ownedState is an optional
// PostgresChallengeProductionState* for ServerMain lifetime; nullptr uses
// stack-local state (tests).
std::unique_ptr<ServerCommandDispatcher> postgresProductionBootstrap(
    session::SessionRegistry& registry,
    std::string& error,
    void* ownedState);

}

#endif

#include "ServerBootstrap.h"

#include <cstdlib>
#include <cstring>

namespace server {

namespace {

bool isTruthyEnvValue(const char* value) {
    if (value == nullptr || value[0] == '\0') {
        return false;
    }
    return std::strcmp(value, "1") == 0 || std::strcmp(value, "true") == 0;
}

}

bool ServerBootstrap::challengeAuthEnabledFromEnvironment() {
    return isTruthyEnvValue(std::getenv("SEED_CHALLENGE_AUTH"));
}

std::unique_ptr<ServerCommandDispatcher> ServerBootstrap::createCommandDispatcher(
    session::SessionRegistry& registry,
    ChallengeAuthBundle* challengeAuth) {
    if (challengeAuth == nullptr) {
        return std::make_unique<ServerCommandDispatcher>(registry);
    }
    return std::make_unique<ServerCommandDispatcher>(
        registry, challengeAuth->auth, challengeAuth->gameplay);
}

std::unique_ptr<ServerCommandDispatcher> ServerBootstrap::createProductionCommandDispatcher(
    session::SessionRegistry& registry,
    std::string& error) {
    error.clear();
    if (challengeAuthEnabledFromEnvironment()) {
        error = "seed_server: SEED_CHALLENGE_AUTH is set but Postgres "
                "challenge adapters are not wired yet (LISS-0147)";
        return nullptr;
    }
    return createCommandDispatcher(registry, nullptr);
}

}

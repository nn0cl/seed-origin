#include "ServerBootstrap.h"

#include <cstdlib>
#include <cstring>
#include <memory>

#include "ChallengeSessionLogin.h"
#include "PostgresProductionBootstrapBridge.h"

namespace server {

namespace {

bool isTruthyEnvValue(const char* value) {
    if (value == nullptr || value[0] == '\0') {
        return false;
    }
    return std::strcmp(value, "1") == 0 || std::strcmp(value, "true") == 0;
}

std::unique_ptr<ChallengeSessionLoginService>& retainedHookAuth() {
    static std::unique_ptr<ChallengeSessionLoginService> instance;
    return instance;
}

bool hasCompleteTestHook(const ChallengeProductionTestHook& hook) {
    return hook.challenges != nullptr &&
           hook.sessions != nullptr &&
           hook.keys != nullptr &&
           hook.clock != nullptr &&
           hook.gameplay != nullptr;
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
    std::string& error,
    ChallengeProductionTestHook* testHook,
    void* postgresOwnedState) {
    error.clear();
    if (!challengeAuthEnabledFromEnvironment()) {
        return createCommandDispatcher(registry, nullptr);
    }

    if (testHook != nullptr && hasCompleteTestHook(*testHook)) {
        std::unique_ptr<ChallengeSessionLoginService>& auth =
            retainedHookAuth();
        auth =
            std::make_unique<ChallengeSessionLoginService>(
                *testHook->challenges, *testHook->sessions, *testHook->keys,
                *testHook->clock);
        ChallengeAuthBundle bundle = {*auth, *testHook->gameplay};
        return createCommandDispatcher(registry, &bundle);
    }

    return postgresProductionBootstrap(registry, error, postgresOwnedState);
}

}

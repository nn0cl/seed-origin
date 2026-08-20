#include "ServerCommandDispatcher.h"
#include "ChatCommandHandler.h"
#include "CombatCommandHandler.h"
#include "DisconnectCommandHandler.h"
#include "MovementCommandHandler.h"
#include "ServerCommandErrors.h"

namespace server {

namespace {

session::SessionInfo emptySession() {
    return {0, 0, std::string(), false};
}

bool requireActiveSession(session::SessionRegistry& registry,
                          int64_t sessionId,
                          const char* commandName,
                          std::string& error) {
    if (registry.isActive(sessionId)) {
        return true;
    }
    error = std::string(commandName) + " requires an active session";
    return false;
}

}

ServerCommandDispatcher::ServerCommandDispatcher(session::SessionRegistry& registry)
    : registry(registry),
      inputQueue(0),
      snapshotRequests(0),
      challengeAuth(0),
      gameplaySessions(0) {}

ServerCommandDispatcher::ServerCommandDispatcher(session::SessionRegistry& registry,
                                                 WorldInputQueue& queue)
    : registry(registry),
      inputQueue(&queue),
      snapshotRequests(0),
      challengeAuth(0),
      gameplaySessions(0) {}

ServerCommandDispatcher::ServerCommandDispatcher(session::SessionRegistry& registry,
                                                 ChallengeSessionLoginService& auth,
                                                 GameplaySessionPort& gameplay)
    : registry(registry),
      inputQueue(0),
      snapshotRequests(0),
      challengeAuth(&auth),
      gameplaySessions(&gameplay) {}

void ServerCommandDispatcher::bindWorldInputQueue(WorldInputQueue& queue) {
    inputQueue = &queue;
}

void ServerCommandDispatcher::beginFrame(uint64_t worldTick) {
    rateLimiter.beginFrame(worldTick);
    snapshotRequests = 0;
}

void ServerCommandDispatcher::forgetSession(int64_t sessionId) {
    rateLimiter.forgetSession(sessionId);
}

void ServerCommandDispatcher::clearRateLimits() {
    rateLimiter.clear();
}

session::SessionRegistry& ServerCommandDispatcher::sessionRegistry() {
    return registry;
}

std::size_t ServerCommandDispatcher::snapshotRequestCount() const {
    return snapshotRequests;
}

CommandDispatchResult ServerCommandDispatcher::dispatch(
    const network::NetworkCommand& command) {
    CommandDispatchResult result = {false, command.type, emptySession(), std::string()};

    if (command.type == network::CommandType::Login) {
        return dispatchLogin(command);
    }

    if (command.type == network::CommandType::Chat) {
        if (inputQueue == 0) {
            result.error = "chat world input queue is not bound";
            return result;
        }
        if (!requireActiveSession(registry, command.sessionId, "chat", result.error)) {
            return result;
        }
        if (!rateLimiter.allow(command.sessionId, command.type)) {
            result.error = "chat command rate limit exceeded";
            return result;
        }
        ChatCommandHandler handler(*inputQueue);
        result.accepted = handler.handle(command, result.error);
        return result;
    }

    if (command.type == network::CommandType::Attack ||
        command.type == network::CommandType::CastSpell) {
        if (inputQueue == 0) {
            result.error = "combat world input queue is not bound";
            return result;
        }
        if (!requireActiveSession(registry, command.sessionId, "combat", result.error)) {
            return result;
        }
        if (!rateLimiter.allow(command.sessionId, command.type)) {
            result.error = "combat command rate limit exceeded";
            return result;
        }
        CombatCommandHandler handler(*inputQueue);
        const CombatCommandResult combat = handler.handle(command);
        result.accepted = combat.accepted;
        result.error = combat.error;
        return result;
    }

    if (command.type == network::CommandType::Move) {
        if (inputQueue == nullptr) {
            result.error = "movement world input queue is not bound";
            return result;
        }
        if (!requireActiveSession(registry, command.sessionId, "movement", result.error)) {
            return result;
        }
        if (!rateLimiter.allow(command.sessionId, command.type)) {
            result.error = "movement command rate limit exceeded";
            return result;
        }
        MovementCommandHandler handler(*inputQueue);
        const MovementResult movement = handler.handle(command);
        result.accepted = movement.accepted;
        result.error = movement.error;
        return result;
    }

    if (command.type == network::CommandType::RequestSnapshot) {
        if (!requireActiveSession(registry, command.sessionId,
                                  "snapshot request", result.error)) {
            return result;
        }
        if (!rateLimiter.allow(command.sessionId, command.type)) {
            result.error = "snapshot request rate limit exceeded";
            return result;
        }
        ++snapshotRequests;
        result.accepted = true;
        return result;
    }

    if (command.type == network::CommandType::Disconnect) {
        if (!requireActiveSession(registry, command.sessionId, "disconnect", result.error)) {
            return result;
        }
        if (!rateLimiter.allow(command.sessionId, command.type)) {
            result.error = "disconnect command rate limit exceeded";
            return result;
        }
        DisconnectCommandHandler handler(registry);
        const DisconnectResult ended = handler.handle(command);
        result.accepted = ended.accepted;
        result.session = ended.session;
        result.error = ended.error;
        if (result.accepted) {
            rateLimiter.forgetSession(command.sessionId);
        }
        return result;
    }

    result.error = "command handler is not implemented";
    return result;
}

bool ServerCommandDispatcher::usesChallengeLogin() const {
    return challengeAuth != 0 && gameplaySessions != 0;
}

CommandDispatchResult ServerCommandDispatcher::dispatchLogin(
    const network::NetworkCommand& command) {
    CommandDispatchResult result = {false, command.type, emptySession(), std::string()};
    if (!usesChallengeLogin()) {
        result.error = kChallengeAuthRequiredError;
        return result;
    }

    ChallengeLoginCommandHandler challengeLogin(*challengeAuth, *gameplaySessions);
    const ChallengeLoginCommandResult login = challengeLogin.handle(command);
    result.accepted = login.accepted;
    result.session = login.session;
    result.error = login.error;
    result.playerSessionKey = login.playerSessionKey;
    return result;
}

std::vector<CommandDispatchResult> ServerCommandDispatcher::dispatchAll(
    const std::vector<network::NetworkCommand>& commands) {
    std::vector<CommandDispatchResult> results;
    results.reserve(commands.size());
    for (std::vector<network::NetworkCommand>::const_iterator it = commands.begin();
         it != commands.end(); ++it) {
        results.push_back(dispatch(*it));
    }
    return results;
}

}

#include "ChallengeLoginCommandHandler.h"

namespace server {

ChallengeLoginCommandHandler::ChallengeLoginCommandHandler(
    ChallengeSessionLoginService& auth,
    GameplaySessionPort& gameplaySessions)
    : auth(auth), gameplaySessions(gameplaySessions) {}

ChallengeLoginCommandResult ChallengeLoginCommandHandler::handle(
    const network::NetworkCommand& command) {
    if (command.type != network::CommandType::Login) {
        return rejected("command is not a login");
    }

    std::string validationError;
    if (!network::validateCommand(command, validationError)) {
        return rejected(validationError);
    }

    ChallengeKey challenge;
    challenge.value = command.payload;
    const ChallengeLoginResult authResult = auth.loginWithChallenge(challenge);
    if (!authResult.accepted) {
        const std::string reason =
            authResult.reason.empty() ? "invalid_challenge" : authResult.reason;
        return rejected(reason);
    }

    const session::SessionInfo session =
        gameplaySessions.openAuthenticated(authResult.userId);
    if (session.internalId == 0) {
        return rejected("session id allocation failed");
    }

    return accepted(authResult.userId, authResult.sessionKey, session);
}

ChallengeLoginCommandResult ChallengeLoginCommandHandler::rejected(
    const std::string& error) {
    ChallengeLoginCommandResult result;
    result.accepted = false;
    result.error = error;
    result.session.internalId = 0;
    result.session.aliasId = 0;
    result.session.claimedId.clear();
    result.session.authenticated = false;
    result.userId = 0;
    return result;
}

ChallengeLoginCommandResult ChallengeLoginCommandHandler::accepted(
    int64_t userId,
    const PlayerSessionKey& sessionKey,
    const session::SessionInfo& session) {
    ChallengeLoginCommandResult result;
    result.accepted = true;
    result.userId = userId;
    result.playerSessionKey = sessionKey;
    result.session = session;
    return result;
}

}

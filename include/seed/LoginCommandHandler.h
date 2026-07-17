#ifndef SEED_LOGIN_COMMAND_HANDLER_H
#define SEED_LOGIN_COMMAND_HANDLER_H

#include "NetworkCommand.h"
#include "SessionRegistry.h"

namespace server {

struct LoginResult {
    bool accepted;
    std::string error;
    session::SessionInfo session;
};

class LoginCommandHandler {
public:
    explicit LoginCommandHandler(session::SessionRegistry& registry);
    LoginResult handle(const network::NetworkCommand& command);
    session::SessionRegistry& sessionRegistry();

private:
    session::SessionRegistry& registry;
};

}

#endif

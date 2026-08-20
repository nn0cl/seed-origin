#include "RandomSessionKeyIssuer.h"

#include <cstdlib>
#include <sstream>

namespace server {

PlayerSessionKey RandomSessionKeyIssuer::issue() {
    std::ostringstream buffer;
    buffer << "session-" << std::rand() << "-" << std::rand();
    return PlayerSessionKey{buffer.str()};
}

}

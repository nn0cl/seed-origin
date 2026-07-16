#include <cassert>

#include "Connection.h"

namespace connection_tests {

void close_is_idempotent() {
    Connection connection;
    assert(!connection.isOpen());
    assert(connection.closeSocket());
    assert(!connection.isOpen());
}

} // namespace connection_tests

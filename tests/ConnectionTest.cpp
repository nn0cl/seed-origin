#include <cassert>

#include "Connection.h"

namespace connection_tests {

void close_is_idempotent() {
    Connection connection;
    assert(!connection.isOpen());
    assert(connection.closeSocket());
    assert(!connection.isOpen());
}

void reports_no_pending_client_without_blocking() {
    Connection connection;
    int clientSocket = -1;
    assert(connection.open(0));
    assert(connection.acceptClient(clientSocket) == AcceptStatus::NoPendingClient);
    assert(clientSocket == -1);
    assert(connection.closeSocket());
}

} // namespace connection_tests

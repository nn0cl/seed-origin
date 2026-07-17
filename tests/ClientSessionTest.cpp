#include <cassert>
#include <sys/socket.h>
#include <unistd.h>

#include "ClientSession.h"
#include "NetworkFrameCodec.h"
#include "LoginResponseCodec.h"

namespace client_session_tests {

void receives_complete_commands_and_closes_peer_cleanly() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    server::ClientSession session(sockets[1]);

    network::NetworkCommand command = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0, "player"};
    std::vector<uint8_t> frame;
    std::string error;
    assert(network::encodeFrame(command, frame, error));
    assert(::send(sockets[0], frame.data(), frame.size(), 0) ==
           static_cast<ssize_t>(frame.size()));

    std::vector<network::NetworkCommand> commands;
    assert(session.receive(commands, error) == server::ReceiveStatus::Commands);
    assert(commands.size() == 1);
    assert(commands[0].type == network::CommandType::Login);
    ::close(sockets[0]);
}

void flushes_one_queued_response_to_peer() {
    int sockets[2] = {-1, -1};
    assert(::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    server::ClientSession session(sockets[1]);
    const network::LoginResponse response = {
        network::CURRENT_PROTOCOL_VERSION,
        network::LoginResponseStatus::Accepted,
        42,
        std::string()
    };
    std::vector<uint8_t> frame;
    std::string error;
    assert(network::encodeLoginResponseFrame(response, frame, error));
    assert(session.enqueueFrame(frame, error));
    assert(session.flushOutbound(error) == server::SendStatus::Sent);
    std::vector<uint8_t> received(frame.size(), 0);
    assert(::recv(sockets[0], received.data(), received.size(), 0) ==
           static_cast<ssize_t>(received.size()));
    assert(received == frame);
    ::close(sockets[0]);
}

} // namespace client_session_tests

#include <arpa/inet.h>
#include <cassert>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "ChallengeSessionLogin.h"
#include "FieldSessionPresence.h"
#include "GameplaySessionPort.h"
#include "LoginFieldSpawnSettings.h"
#include "LoginResponseCodec.h"
#include "NetworkFrameCodec.h"
#include "ServerCommandDispatcher.h"
#include "ServerRuntime.h"

namespace server_runtime_challenge_login_tests {
namespace {

class FixedWallClock : public server::WallClock {
public:
    explicit FixedWallClock(int64_t now) : nowUnix(now) {}
    int64_t nowUnixSeconds() const { return nowUnix; }
    int64_t nowUnix;
};

class FakeChallengeClaimPort : public server::PlayerChallengeClaimPort {
public:
    struct Row {
        int64_t userId;
        int64_t expiresAtUnix;
        bool claimed;
    };

    std::map<std::string, Row> rows;

    bool claim(const server::ChallengeKey& key,
               int64_t nowUnixSeconds,
               int64_t& outUserId) {
        std::map<std::string, Row>::iterator it = rows.find(key.value);
        if (it == rows.end() || it->second.claimed ||
            it->second.expiresAtUnix <= nowUnixSeconds) {
            return false;
        }
        it->second.claimed = true;
        outUserId = it->second.userId;
        return true;
    }
};

class FakeSessionStore : public server::PlayerSessionStorePort {
public:
    struct Row {
        int64_t userId;
        int64_t expiresAtUnix;
    };

    std::map<std::string, Row> rows;

    void create(int64_t userId,
                const server::PlayerSessionKey& key,
                int64_t expiresAtUnix) {
        Row row;
        row.userId = userId;
        row.expiresAtUnix = expiresAtUnix;
        rows[key.value] = row;
    }

    bool extend(const server::PlayerSessionKey&, int64_t, int64_t) { return false; }

    bool isActive(const server::PlayerSessionKey&, int64_t) const { return false; }
};

class FixedKeyIssuer : public server::SessionKeyIssuer {
public:
    explicit FixedKeyIssuer(const std::string& value) : nextValue(value) {}

    server::PlayerSessionKey issue() {
        server::PlayerSessionKey key;
        key.value = nextValue;
        return key;
    }

    std::string nextValue;
};

class FakeGameplaySessionPort : public server::GameplaySessionPort {
public:
    session::SessionInfo openAuthenticated(int64_t) {
        session::SessionInfo info;
        info.internalId = 21;
        info.aliasId = 0;
        info.claimedId.clear();
        info.authenticated = true;
        return info;
    }
};

int connectLoopback(uint16_t port) {
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        ::close(fd);
        return -1;
    }
    return fd;
}

} // namespace

void process_frame_writes_player_session_key_on_accepted_login() {
    const int64_t now = 1700000000;
    FixedWallClock clock(now);
    FakeChallengeClaimPort challenges;
    FakeChallengeClaimPort::Row row;
    row.userId = 42;
    row.expiresAtUnix = now + 120;
    row.claimed = false;
    challenges.rows["challenge-ok"] = row;
    FakeSessionStore sessions;
    FixedKeyIssuer keys("player-session-1");
    server::ChallengeSessionLoginService auth(challenges, sessions, keys, clock);
    FakeGameplaySessionPort gameplay;
    session::SessionRegistry registry;
    server::ServerCommandDispatcher dispatcher(registry, auth, gameplay);

    server::LoginFieldSpawnSettings spawn;
    spawn.playerName = "Hero";
    server::FieldSessionPresence::useSpawnSettings(spawn);

    server::ServerRuntime runtime;
    assert(runtime.start(0));
    const uint16_t port = runtime.listeningPort();
    const int client = connectLoopback(port);
    assert(client >= 0);

    uint64_t connectionId = 0;
    std::string error;
    AcceptStatus accepted = AcceptStatus::NoPendingClient;
    for (int i = 0; i < 16 && accepted == AcceptStatus::NoPendingClient; ++i) {
        accepted = runtime.acceptPendingClient(connectionId, error);
    }
    assert(accepted == AcceptStatus::Accepted);

    const network::NetworkCommand login = {
        network::CURRENT_PROTOCOL_VERSION, network::CommandType::Login, 0,
        "challenge-ok"};
    std::vector<uint8_t> loginFrame;
    assert(network::encodeFrame(login, loginFrame, error));
    assert(::send(client, loginFrame.data(), loginFrame.size(), 0) ==
           static_cast<ssize_t>(loginFrame.size()));

    server::ServerFrameResult frame = {};
    for (int i = 0; i < 8 && frame.newAuthenticatedSessions == 0; ++i) {
        frame = runtime.processFrame(dispatcher, error);
        assert(error.empty());
    }
    assert(frame.newAuthenticatedSessions == 1);

    uint8_t buffer[256];
    const ssize_t n = ::recv(client, buffer, sizeof(buffer), 0);
    assert(n > 0);
    network::LoginResponse response = {};
    assert(network::decodeLoginResponseFrame(
        std::vector<uint8_t>(buffer, buffer + n), response, error));
    assert(response.status == network::LoginResponseStatus::Accepted);
    assert(response.payload == "player-session-1");

    ::close(client);
    assert(runtime.stop(registry));
}

} // namespace server_runtime_challenge_login_tests

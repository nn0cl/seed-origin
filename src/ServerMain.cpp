#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "Field.h"
#include "ServerCommandDispatcher.h"
#include "ServerRuntime.h"
#include "SessionRegistry.h"
#include "WorldFrameApplier.h"

#ifdef SEED_HAVE_LIBPQXX
#include "PostgresIdentityAliasStore.h"
#endif

namespace {

// 20 world ticks per second, matching the WorldInput tick rate assumed by
// WP-0007's already-implemented Canonical slices (movement, combat, NPC AI).
constexpr std::chrono::milliseconds kWorldTickInterval(50);

volatile std::sig_atomic_t g_shutdownRequested = 0;

void handleShutdownSignal(int) {
    g_shutdownRequested = 1;
}

std::unique_ptr<session::IdentityAliasStore> selectIdentityAliasStore() {
#ifdef SEED_HAVE_LIBPQXX
    std::unique_ptr<session::PostgresIdentityAliasStore> postgresStore =
        session::PostgresIdentityAliasStore::fromEnvironment();
    if (postgresStore) {
        std::cout << "identity alias store: PostgreSQL (SEED_IDENTITY_DB_URL)\n";
        return postgresStore;
    }
#endif
    std::cout << "identity alias store: in-memory (SEED_IDENTITY_DB_URL not set)\n";
    return nullptr;
}

} // namespace

int main(int argc, char** argv) {
    uint16_t port = 40000;
    if (argc >= 2) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::signal(SIGINT, handleShutdownSignal);
    std::signal(SIGTERM, handleShutdownSignal);

    std::unique_ptr<session::IdentityAliasStore> aliasStore = selectIdentityAliasStore();
    std::unique_ptr<session::SessionRegistry> registry = aliasStore
        ? std::make_unique<session::SessionRegistry>(*aliasStore)
        : std::make_unique<session::SessionRegistry>();

    server::ServerCommandDispatcher dispatcher(*registry);
    server::ServerRuntime runtime;
    server::WorldFrameApplier applier(*Field::getInstance());

    if (!runtime.start(port)) {
        std::cerr << "seed_server: failed to start on port " << port << "\n";
        return 1;
    }
    std::cout << "seed_server: listening on port " << port << "\n";

    while (g_shutdownRequested == 0) {
        const std::chrono::steady_clock::time_point tickStart =
            std::chrono::steady_clock::now();

        std::string frameError;
        const server::ServerFrameResult frame = runtime.processFrame(dispatcher, frameError);
        if (!frameError.empty()) {
            std::cerr << "seed_server: frame error: " << frameError << "\n";
        }

        std::vector<network::WorldUpdate> updates;
        std::string applyError;
        applier.apply(server::WorldFrameInputs{frame.worldTick, frame.inputs}, updates,
                      applyError);
        if (!applyError.empty()) {
            std::cerr << "seed_server: world apply error: " << applyError << "\n";
        }

        std::string publishError;
        std::vector<server::MovementAck> publishAcks = applier.ownerMovementAcks();
        std::string snapshotError;
        if (!applier.capturePublicSnapshotIfNewSessions(
                frame.newAuthenticatedSessions, frame.worldTick, updates,
                publishAcks, snapshotError) &&
            !snapshotError.empty()) {
            std::cerr << "seed_server: snapshot error: " << snapshotError
                      << "\n";
        }
        runtime.publishWorldUpdates(updates, publishAcks, publishError);
        if (!publishError.empty()) {
            std::cerr << "seed_server: publish error: " << publishError << "\n";
        }

        const std::chrono::steady_clock::duration elapsed =
            std::chrono::steady_clock::now() - tickStart;
        if (elapsed < kWorldTickInterval) {
            std::this_thread::sleep_for(kWorldTickInterval - elapsed);
        }
    }

    std::cout << "seed_server: shutting down\n";
    runtime.stop(*registry);
    return 0;
}

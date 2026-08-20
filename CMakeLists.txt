cmake_minimum_required(VERSION 3.15)

project(seed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SEED_CORE_SOURCES
    src/Action.cpp
    src/ActionQueue.cpp
    src/ServerTick.cpp
    src/WorldUpdate.cpp
    src/WorldUpdateSequence.cpp
    src/WorldUpdateSync.cpp
    src/WorldUpdateFrameCodec.cpp
    src/Buff.cpp
    src/Connection.cpp
    src/ClientSession.cpp
    src/Data.cpp
    src/Field.cpp
    src/FileManager.cpp
    src/Manager.cpp
    src/NetworkCommand.cpp
    src/ServerRuntime.cpp
    src/NetworkFrameCodec.cpp
    src/FrameAccumulator.cpp
    src/LoginCommandHandler.cpp
    src/DisconnectCommandHandler.cpp
    src/ServerCommandDispatcher.cpp
    src/CommandRateLimiter.cpp
    src/LoginResponse.cpp
    src/LoginResponseCodec.cpp
    src/DisconnectResponse.cpp
    src/DisconnectResponseCodec.cpp
    src/OutboundFrameQueue.cpp
    src/SessionLifecycle.cpp
    src/WorldFrameUpdateBuilder.cpp
    src/WorldFrameApplier.cpp
    src/MovementIntentQueue.cpp
    src/MovementSimulation.cpp
    src/ClientInputSequence.cpp
    src/PendingInputBuffer.cpp
    src/LocalMovementPredictor.cpp
    src/RemotePlayerPoseStore.cpp
    src/WorldInputQueue.cpp
    src/WorldInputTick.cpp
    src/CombatCommandHandler.cpp
    src/ChatCommandHandler.cpp
    src/EnvironmentEther.cpp
    src/WorldSnapshotBuilder.cpp
    src/FieldSessionPresence.cpp
    src/InMemoryAuthenticatedPlayerIdStub.cpp
    src/ClientEnvironmentState.cpp
    src/ClientWorldSnapshotApplier.cpp
    src/ClientHazardEffectQueue.cpp
    src/WorldUpdateAccumulator.cpp
    src/ClientWorldUpdateReceiver.cpp
    src/ClientInboundDemux.cpp
    src/ClientTransportShell.cpp
    src/MovementCommandHandler.cpp
    src/Npc.cpp
    src/NpcAi.cpp
    src/Player.cpp
    src/Position.cpp
    src/SeedBinary.cpp
    src/SessionRegistry.cpp
    src/IdentityAliasStore.cpp
    src/AdminSessionStore.cpp
    src/AdminLoginLockout.cpp
    src/Status.cpp
)

add_library(seed_core ${SEED_CORE_SOURCES})
target_include_directories(seed_core PUBLIC include/seed)

add_executable(seed_cli src/main.cpp)
target_link_libraries(seed_cli PRIVATE seed_core)

# LISS-0056 composition root: fixed-tick accept/dispatch/apply/publish loop.
add_executable(seed_server src/ServerMain.cpp)
target_link_libraries(seed_server PRIVATE seed_core)

enable_testing()

# Optional PostgreSQL-backed IdentityAliasStore adapter (LISS-0143, ADR 0016).
# seed_core and seed_tests never depend on libpqxx: the in-memory adapter
# remains the default/test baseline whether or not libpqxx is installed.
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBPQXX QUIET libpqxx)
endif()

if(LIBPQXX_FOUND)
    message(STATUS "libpqxx found: building PostgresIdentityAliasStore adapter")

    add_library(seed_postgres src/PostgresIdentityAliasStore.cpp src/AdminAuthStore.cpp)
    target_include_directories(seed_postgres PUBLIC include/seed)
    target_link_libraries(seed_postgres PUBLIC seed_core)
    target_include_directories(seed_postgres PUBLIC ${LIBPQXX_INCLUDE_DIRS})
    target_link_directories(seed_postgres PUBLIC ${LIBPQXX_LIBRARY_DIRS})
    target_link_libraries(seed_postgres PUBLIC ${LIBPQXX_LIBRARIES})
    target_compile_options(seed_postgres PUBLIC ${LIBPQXX_CFLAGS_OTHER})

    target_link_libraries(seed_server PRIVATE seed_postgres)
    target_compile_definitions(seed_server PRIVATE SEED_HAVE_LIBPQXX)

    add_executable(seed_postgres_tests
        tests/PostgresIdentityAliasStoreTest.cpp
    )
    target_link_libraries(seed_postgres_tests PRIVATE seed_postgres)
    add_test(NAME seed_postgres_tests COMMAND seed_postgres_tests)
else()
    message(STATUS "libpqxx not found: PostgresIdentityAliasStore adapter is skipped")
endif()

# Optional admin HTTP surface (LISS-0144, ADR 0017). Requires both libpqxx
# and cpp-httplib; seed_core/seed_postgres never depend on HTTP.
find_package(httplib CONFIG QUIET)

if(LIBPQXX_FOUND AND httplib_FOUND)
    message(STATUS "cpp-httplib found: building seed_admin")
    add_executable(seed_admin src/AdminMain.cpp)
    target_link_libraries(seed_admin PRIVATE seed_postgres httplib::httplib)
elseif(LIBPQXX_FOUND)
    message(STATUS "cpp-httplib not found: seed_admin is skipped")
endif()

add_executable(seed_tests
    tests/ActionInputTest.cpp
    tests/ActionQueueTest.cpp
    tests/ServerTickTest.cpp
    tests/WorldUpdateTest.cpp
    tests/WorldUpdateSequenceTest.cpp
    tests/WorldUpdateSyncTest.cpp
    tests/WorldUpdateFrameCodecTest.cpp
    tests/ConnectionTest.cpp
    tests/ClientSessionTest.cpp
    tests/FieldStateTest.cpp
    tests/FieldSessionPresenceTest.cpp
    tests/PlayerOwnershipTest.cpp
    tests/SeedBinaryTest.cpp
    tests/SessionRegistryTest.cpp
    tests/IdentityAliasStoreTest.cpp
    tests/NetworkCommandTest.cpp
    tests/ServerRuntimeTest.cpp
    tests/NetworkFrameCodecTest.cpp
    tests/LoginResponseCodecTest.cpp
    tests/DisconnectResponseCodecTest.cpp
    tests/OutboundFrameQueueTest.cpp
    tests/SessionLifecycleTest.cpp
    tests/WorldFrameUpdateBuilderTest.cpp
    tests/WorldFrameApplierTest.cpp
    tests/MovementIntentQueueTest.cpp
    tests/WorldInputQueueTest.cpp
    tests/WorldInputTickTest.cpp
    tests/CombatCommandHandlerTest.cpp
    tests/ChatCommandHandlerTest.cpp
    tests/CombatResolutionEventTest.cpp
    tests/CombatResourceTest.cpp
    tests/EnvironmentEtherTest.cpp
    tests/WorldSnapshotBuilderTest.cpp
    tests/ClientWorldSnapshotApplierTest.cpp
    tests/ClientHazardEffectQueueTest.cpp
    tests/ClientWorldUpdateReceiverTest.cpp
    tests/FrameAccumulatorTest.cpp
    tests/LoginCommandHandlerTest.cpp
    tests/DisconnectCommandHandlerTest.cpp
    tests/ServerCommandDispatcherTest.cpp
    tests/CommandRateLimiterTest.cpp
    tests/MovementCommandHandlerTest.cpp
    tests/ClientInputSequenceTest.cpp
    tests/LocalMovementPredictorTest.cpp
    tests/RemotePlayerPoseStoreTest.cpp
    tests/ClientPredictionSyncTest.cpp
    tests/RequestSnapshotCommandTest.cpp
    tests/ClientInboundDemuxTest.cpp
    tests/ClientTransportShellTest.cpp
    tests/ClientTransportShellTimeoutTest.cpp
    tests/NpcCombatTargetTest.cpp
    tests/NpcAiTest.cpp
    tests/StatusSaturationTest.cpp
    tests/TestMain.cpp
)
target_link_libraries(seed_tests PRIVATE seed_core)
add_test(NAME seed_tests COMMAND seed_tests)

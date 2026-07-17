cmake_minimum_required(VERSION 3.15)

project(seed LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
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
    src/ServerCommandDispatcher.cpp
    src/LoginResponse.cpp
    src/LoginResponseCodec.cpp
    src/OutboundFrameQueue.cpp
    src/SessionLifecycle.cpp
    src/WorldFrameUpdateBuilder.cpp
    src/WorldFrameApplier.cpp
    src/MovementIntentQueue.cpp
    src/WorldInputQueue.cpp
    src/WorldInputTick.cpp
    src/CombatCommandHandler.cpp
    src/ChatCommandHandler.cpp
    src/EnvironmentEther.cpp
    src/WorldSnapshotBuilder.cpp
    src/ClientEnvironmentState.cpp
    src/ClientWorldSnapshotApplier.cpp
    src/ClientHazardEffectQueue.cpp
    src/WorldUpdateAccumulator.cpp
    src/ClientWorldUpdateReceiver.cpp
    src/MovementCommandHandler.cpp
    src/Player.cpp
    src/Position.cpp
    src/SeedBinary.cpp
    src/SessionRegistry.cpp
    src/Status.cpp
)

add_library(seed_core ${SEED_CORE_SOURCES})
target_include_directories(seed_core PUBLIC include/seed)

add_executable(seed_cli src/main.cpp)
target_link_libraries(seed_cli PRIVATE seed_core)

enable_testing()

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
    tests/PlayerOwnershipTest.cpp
    tests/SeedBinaryTest.cpp
    tests/SessionRegistryTest.cpp
    tests/NetworkCommandTest.cpp
    tests/ServerRuntimeTest.cpp
    tests/NetworkFrameCodecTest.cpp
    tests/LoginResponseCodecTest.cpp
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
    tests/ServerCommandDispatcherTest.cpp
    tests/MovementCommandHandlerTest.cpp
    tests/StatusSaturationTest.cpp
    tests/TestMain.cpp
)
target_link_libraries(seed_tests PRIVATE seed_core)
add_test(NAME seed_tests COMMAND seed_tests)

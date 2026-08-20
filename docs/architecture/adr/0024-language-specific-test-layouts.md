# ADR 0024: Language-Specific Test Layouts for C++ and Kotlin Services

## Status

Accepted — ratified by the Adjudicator on 2026-07-23.

## Context

The repository currently contains a C++20 core/server validated by CMake/CTest
under `tests/`. LISS-0146 and ADR 0023 add `seed_auth` as a Kotlin + Spring
Boot service built with Gradle. Kotlin service tests must use the service's
JUnit/Gradle toolchain and must not be represented as C++ translation units or
linked into `seed_tests`.

## Decision

Test sources follow the language and build boundary of the component under
test:

- C++ core/server and C++ adapters: `tests/`, registered with CTest through
  the root `CMakeLists.txt`.
- Kotlin/Spring Boot services such as `seed_auth`: under the service's
  `backend/src/test/kotlin/` tree, executed by the service's Gradle test task.
- React/frontend tests: under the relevant frontend project and its selected
  frontend test task after the frontend test framework is approved.
- Cross-process E2E tests: a separate test layer after a runnable deployment
  exists; they are not substitutes for domain or use-case tests.

Phase 1 Red tests must be placed in the target component's test tree and use
that component's normal deterministic test runner. Ports and fake adapters are
preferred; real PostgreSQL, HTTP, or external services are not required for
core Phase 1 behavior.

The root CMake/CTest target remains responsible only for the C++ core/server
test graph. Kotlin service tests are not added to `seed_tests`.

## Consequences

Positive:

- Kotlin tests can use JUnit, Gradle, Spring test support, and Kotlin types
  without crossing the C++ build boundary.
- C++ CTest remains independent of JVM toolchain availability.
- Test ownership and failure reporting follow the component being tested.

Negative / follow-up:

- CI must eventually invoke both the C++ CTest job and each service's Gradle
  test task.
- Each new service must document its Gradle wrapper, test task, and optional
  integration-test boundary before implementation begins.

## Related documents

- `docs/architecture/testing-strategy.md`
- `docs/architecture/project-structure.md`
- `docs/issues/LISS-0146-user-registration-and-auth-service.md`
- `docs/architecture/adr/0023-player-auth-session-flow-details.md`

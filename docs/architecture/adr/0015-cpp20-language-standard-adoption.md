# ADR 0015: Adopt C++20 as the Project Language Standard

## Status

Accepted

## Ratification

Ratified as pre-approved by the Adjudicator for this migration initiative on
2026-07-17. Scope, safety constraints, and staged execution are recorded below.

## Context

The core and server are currently built as C++11 (`CMAKE_CXX_STANDARD 11`,
`README`/architecture docs state "C++11 runtime"). The codebase has since grown
to a networked, tick-driven server with many small value types, queues, and
codecs. The C++11 baseline forces older idioms that raise human review cost and
hide correctness risks:

- C-style headers (`<stdint.h>`, `<stddef.h>`) instead of `<cstdint>`/`<cstddef>`.
- Manual iterator loops instead of range-based `for`.
- Header constants declared as `static const` (internal linkage per TU) instead
  of `inline constexpr` single-definition constants.
- No `[[nodiscard]]`, `override`, or `= default`/`= delete` discipline in older
  files.
- Ownership-carrying types (for example `Player`) written with body-assignment
  constructors that require default-constructing non-default-constructible
  members, which is both a modernization and a correctness problem.

A recent set of commits also landed on `main` in a non-compiling state (missing
includes, a missing closing parenthesis, and namespace mismatches in tests).
A standard bump must therefore be sequenced *after* the build is restored to a
known-green compile, so migration diffs are attributable and reviewable.

The Adjudicator additionally constrained this initiative: **artifact and test
execution is prohibited**; only source edits, documentation, CMake
configure/build, compiler syntax checks, and static analysis are permitted.
Verification is therefore compile-time and static only for this initiative.

## Dependency Adoption Evidence

No new third-party runtime or test dependency is selected by this ADR.

- Vulnerability/advisory check: Not applicable (no new dependency).
- Version-specific examples checked: AppleClang 21 / Clang toolchain present in
  the environment supports C++20; CMake 3.15+ supports `CMAKE_CXX_STANDARD 20`.
- Troubleshooting and known-issue evidence: C++20 is well established across
  Clang/GCC/MSVC; no exotic features (modules, coroutines) are adopted here.
- Minimal real-file test or fixture plan: existing `tests/` translation units
  are compiled (not executed) as the deterministic gate.
- POC feasibility/result: `CMAKE_CXX_STANDARD 20` configures and the tree
  compiles under the same compiler once the pre-existing breaks are fixed.
- Clean Architecture boundary: unchanged; this is a language-standard and
  idiom decision, not a dependency or layering change.

## Decision

1. The project language standard is **C++20**. `CMakeLists.txt` sets
   `CMAKE_CXX_STANDARD 20`, `CMAKE_CXX_STANDARD_REQUIRED ON`, and
   `CMAKE_CXX_EXTENSIONS OFF`.
2. Architecture documentation that states "C++11" is updated to "C++20".
3. Modernization is **idiom-level and conservative**. Adopted now:
   - `<cstdint>`/`<cstddef>` over C-style headers.
   - `inline constexpr` for header-defined shared constants (single definition).
   - range-based `for` and `auto` where it improves readability.
   - `[[nodiscard]]`, `override`, `= default`, `= delete` where they encode
     intent and prevent misuse.
   - member-initializer-list constructors for ownership-carrying types.
4. **Deferred** (out of scope, tracked as future issues): modules, coroutines,
   `std::span`/`std::optional` API redesigns, `std::format` adoption, and strong
   typed-ID value objects. These change public contracts and require their own
   review.
5. Public wire behavior and protocol constants must not change values. Protocol
   constants are centralized in `include/seed/Protocol.h` and shared by
   `NetworkCommand.h` and `WorldUpdate.h`.
6. Migration is staged so each change is compile-verifiable and attributable:
   (a) restore green compile, (b) bump standard + docs, (c) idiom/safety
   modernization per module.

## Consequences

Positive:

- Lower human review cost through modern, uniform idioms.
- Single-definition constants remove per-TU duplication and ODR ambiguity.
- Ownership and initialization bugs are fixed as part of modernization.
- Header self-containment (each header includes what it uses) is enforced.

Negative:

- Broad, cross-file diffs require careful, staged review.
- Toolchains older than C++20 are no longer supported for this project.
- The Xcode project's dialect setting may drift from CMake until separately
  synchronized (tracked as a follow-up issue).

## Enforcement

Code review should reject:

- Reintroducing `<stdint.h>`/`<stddef.h>` in new or edited files.
- Re-declaring shared header constants as `static const` instead of
  `inline constexpr`.
- Changing protocol constant values under the guise of modernization.
- Introducing deferred C++20 features (modules, coroutines, API redesigns)
  without a dedicated ADR/issue.
- Standard-bump commits that do not compile, or that mix unrelated behavior
  changes into the mechanical migration.

## Related documents

- `docs/architecture/README.md`
- `docs/architecture/seed-implementation.md`
- `docs/architecture/project-structure.md`
- `docs/work-plans/WP-0008-cpp20-migration-and-modernization.md`
- `docs/issues/LISS-0140-restore-green-compile.md`
- `docs/issues/LISS-0141-cpp20-standard-bump.md`
- `docs/issues/LISS-0142-core-idiom-and-memory-safety-modernization.md`

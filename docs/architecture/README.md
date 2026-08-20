# Architecture Overview

The project uses Clean Architecture with a local-first C++ core and a future
POSIX TCP game server. The current runtime has no external service dependency.

The current stack is C++20, CMake/CTest, and Xcode project integration. The
application core and server contracts live in `include/seed/` and `src/`; a
client UI and renderer remain undecided under LISS-0064.

## Layers

### Domain

Pure Player, Position, Status, Action, Field, WorldUpdate, and server-tick
behavior.

Must not depend on:

- any UI framework.
- SQL schemas, ORM structs, vector DB SDKs, or file-system APIs.
- LLM SDKs, cloud AI SDKs, or third-party provider APIs.

### UseCase

Coordinates domain behavior through ports.

Current use-case/application boundaries include:

- session login and internal-ID allocation.
- validated network Command dispatch.
- authoritative movement and future combat/magic application.
- fixed 20Hz ActionQueue processing.

### Ports

Interfaces owned by the application core.

Ports isolate every external resource named in `CLAUDE.md` / `AGENTS.md`
under "External Resources Must Be Ports".

### Adapters

Framework and infrastructure implementations.

Adapters may use framework APIs, infrastructure libraries, DB or vector DB
SDKs, external file layouts, API clients, and provider SDKs.

Adapters must not define business policy.

### Front-End / Delivery

The delivery layer (UI, CLI, HTTP API) presents domain state and collects
user input.

It must not own:

- confidence, trust, or merge policy for AI-derived data.
- validation or secret-storage policy.
- any policy that belongs in a use case.

## Runtime Direction

The current CLI and core run locally. The planned server runs as a C++ POSIX
TCP process. The planned client renderer/UI is replaceable and must communicate
through the versioned network protocol rather than depend on World internals.

## Selected Technology

- C++20 runtime and POSIX socket adapter.
- CMake/CTest and the existing Xcode project as build/test tooling.
- Client renderer/UI: undecided until LISS-0064.
- Dependency management: repository/system tooling only; no runtime package
  manager is currently selected.
- Distribution: custom license permits unmodified redistribution and prohibits
  derivative works; see `LICENSE` and LISS-0092.

## Detailed Rules

- `project-structure.md`: where files belong.
- `testing-strategy.md`: AT-TDD test placement.
- `implementation-readiness.md`: checklist before coding.
- `dependency-policy.md`: package dependency checking policy.
- `ai-request-routing.md`: AI payload selection and task routing.
- `io-reasoning-contracts.md`: AI input/output/reasoning contracts.
- `external-resource-adoption-contract.md`: optional contract for adopting
  AI-generated or human-sourced external content/data resources.
- C++ core/server: `seed-implementation.md`.
- Network protocol: `../specs/network-protocol-v1.md`.

## Accepted Decisions

- `adr/0001-design-first-ai-request-routing.md`
- `adr/0002-input-output-reasoning-contracts.md`
- `adr/0003-ai-human-collaboration-governance.md`
- `adr/0004-human-readable-source-code-quality.md`
- `adr/0005-local-issue-planning.md`
- `adr/0006-prompt-instruction-change-control.md`
- `adr/0007-trunk-oriented-branching.md`
- `adr/0008-template-update-propagation.md`
- `adr/0009-bug-planning-and-ai-usage-records.md`
- `adr/0010-ai-failure-recovery-and-runner-cli-contract.md`
- `adr/0011-external-resource-adoption-contract.md`
- `adr/0013-project-main-branch-and-adjudicator-roles.md`
- `adr/0014-adjudicator-and-developer-role-model.md`
- `adr/0015-cpp20-language-standard-adoption.md`
- `adr/0016-identity-alias-persistence-and-review.md`
- `adr/0017-admin-authentication-and-http-surface.md`
- `adr/0018-registered-player-authentication.md` (supersedes ADR 0016 for
  player identity; ADR 0016 remains for historical record)
- `adr/0022-restore-per-issue-branching.md` (supersedes ADR 0013's
  branching/push decision only; ADR 0013's Adjudicator/Developer
  role-boundary decision remains in effect)
- `adr/0023-player-auth-session-flow-details.md` (registered player session
  lifecycle and native-client game-play login)
- `adr/0024-language-specific-test-layouts.md` (C++ CTest and Kotlin Gradle
  test boundaries)

`adr/0023-player-auth-session-flow-details.md` is Accepted and resolves ADR
0018's deferred follow-ups: native-client game-play login, two-stage key
lifecycle, 30-minute renewable player sessions, and the LISS-0148 progression
constraints.

## Remaining Technology Evaluation

Technology choices still open for ADR decision:

- Static-asset serving for the React admin/auth SPAs (Spring Boot vs.
  Nginx); build tooling and directory placement are decided (LISS-0145/
  0149: Vite, `seed-admin/frontend/` and `seed-auth/frontend/`, separate
  apps, TanStack Query + Zustand + shadcn/ui + Tailwind CSS v4).
- the exact Keep-Alive endpoint and shared session-store component between
  `seed_auth`/`seed_server` remain implementation details for LISS-0146/0147;
  the lifecycle policy itself is accepted (challenge key 1–2 minutes,
  player session key 30 minutes and renewable).
- player progression schema details (LISS-0148) remain to be designed within
  the accepted classic MMORPG-style constraints.
- production client platform (LISS-0064): Unreal Engine is proposed in
  ADR 0021, not yet approved (version, scripting approach, and repository
  placement still open). A separate Godot 4.7/GDScript MVP (ADR 0020, also
  not yet approved) is a throwaway validation client, not the production
  choice.
- snapshot/event wire encoding extension.
- deployment and observability stack.

Retired by ADR 0018 (no longer open — player identity is resolved by
registered accounts, not these): identity-alias persistence schema/driver
selection detail, and the wall-clock-to-WorldTick retention mapping (ADR
0016 decisions 2/... — ADR 0016 itself remains as a historical record, see
LISS-0150 for the formal deprecation Issue).

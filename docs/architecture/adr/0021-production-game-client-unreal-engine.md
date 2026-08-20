# ADR 0021: Production Game Client Uses Unreal Engine

## Status

Proposed — awaiting Adjudicator review before implementation. No code is
written until this ADR is explicitly approved (per the process correction
recorded in ADR 0019).

## Context

`LISS-0064` has tracked "client platform, renderer, and UI framework" as an
open Non-Decision since the project's start. ADR 0020 separately decided a
throwaway **MVP/validation** client in Godot to exercise the server-side
systems quickly. The Adjudicator has now decided the **production** client
— the one actually shipped to players — is built with **Unreal Engine**,
distinct from and not replacing the Godot MVP.

This environment has Unreal Engine MCP tooling available (the
`unreal-engine-skills-for-claude-code` skill set:
`unreal-mcp`, `create-toolset`, `unreal-skill`), which can drive a live
Unreal Editor session once a project exists.

## Decision (proposed)

1. Production client engine: **Unreal Engine** (specific version to be
   confirmed against current stable at implementation time — not pinned by
   this ADR).
2. Relationship to the Godot MVP (ADR 0020): the Godot MVP remains a
   throwaway validation tool for exercising server-side systems early. It
   is not evolved into the production client; Unreal Engine work starts
   fresh against `LISS-0064`'s acceptance criteria (OS targets, renderer,
   transport layer, input layer, asset placement, logging, build/
   distribution) once that Issue's design is completed.
3. Networking: the same principle as ADR 0020 applies — Unreal reuses
   `seed_server`'s existing `NetworkFrameCodec` binary wire format rather
   than requiring server-side protocol changes. Unreal's C++ layer can
   implement the same 16-byte header framing directly (Unreal supports raw
   TCP sockets via its `FSocket`/`Sockets` module), avoiding a translation
   bridge.
4. This ADR does not decide asset pipeline, target platforms beyond "works
   on Apple Silicon for development," or distribution — those remain
   `LISS-0064`'s scope to design before Phase 1.

## Open questions (must not guess)

- Unreal Engine version to target.
- Scripting approach within Unreal: C++ only, Blueprint-heavy, or a mix —
  affects how directly `seed_server`'s C++ types/logic could theoretically
  be shared (not decided whether any sharing is even desired).
- Whether `client/` (already reserved in `project-structure.md`) holds the
  Unreal project directly, given Unreal projects have their own large,
  engine-specific directory conventions (`Content/`, `Source/`, `.uproject`)
  that don't resemble the rest of this repository's layout — may warrant
  its own top-level directory or a separate repository, not decided here.
- Dependency-adoption checklist
  (`docs/architecture/dependency-policy.md`) for Unreal Engine itself has
  not been performed (license terms, version-specific examples,
  troubleshooting depth, POC feasibility).

## Consequences

Positive:

- Production-quality 3D rendering, asset pipeline, and tooling maturity
  well beyond what a hand-rolled or lightweight-engine client would offer.
- MCP tooling already available in this environment can drive Unreal Editor
  actions directly once a project exists.

Negative:

- Introduces a fifth major technology surface this session (after
  PostgreSQL, Kotlin/Spring Boot, React/TypeScript, and Godot), each with
  independent build/run/licensing/operational concerns.
- Unreal Engine carries commercial licensing terms (royalty above a revenue
  threshold) that should be confirmed as acceptable for this project before
  committing further.
- Doubles the client-technology investment during the MVP-to-production
  transition (Godot MVP code is not reused).

## Related documents

- `docs/issues/LISS-0064-client-shell.md`
- `docs/architecture/adr/0020-game-client-mvp-godot.md`
- `include/seed/NetworkFrameCodec.h`

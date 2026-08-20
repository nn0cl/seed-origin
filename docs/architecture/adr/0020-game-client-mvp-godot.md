# ADR 0020: Game Client MVP Uses Godot Engine

## Status

Proposed — awaiting Adjudicator review before implementation. Per the
correction recorded in ADR 0019, no code is written for this ADR until it
is explicitly approved.

## Context

`CLAUDE.md`'s Non-Decisions and LISS-0064 have left "client platform,
renderer, and UI framework" open since the project's start. The Adjudicator
has decided to build a minimal client **MVP** to validate the server-side
work already implemented — connect to `seed_server`, enter the world, and
exercise movement, attack, spells, terrain, and battle end to end on an M2
MacBook Air, with simple 3D polygon characters, sparkle/particle effects,
and a right-click context menu.

This is explicitly an MVP/demo scope, not the final production client
decision. `LISS-0064` (client technology/app shell), `LISS-0128` (client
transport shell), and `LISS-0129` (client play shell) already exist as
generic, tech-undecided issues; this ADR fills in the technology choice for
their MVP slice rather than replacing them.

## Decision (proposed)

1. Engine: **Godot 4.7** (current stable as of 2026-07-18).
2. Scripting: **GDScript**, not C#, for the MVP — avoids adding a .NET
   runtime dependency for a throwaway/validation-scope client, consistent
   with this project's preference for minimal new toolchains when a
   lighter option covers the need.
3. Networking: Godot's built-in `StreamPeerTCP` connects directly to
   `seed_server`'s existing custom binary protocol
   (`network::NetworkFrameCodec`'s 16-byte header + payload framing, per
   `include/seed/NetworkFrameCodec.h`). The GDScript client re-implements
   the same wire format (version, command type, session id, payload
   length) rather than introducing a second protocol or a translation
   bridge — the server is not changed to accommodate the client.
4. Directory: `client/` (already reserved for this purpose in
   `docs/architecture/project-structure.md`), containing the Godot project.
5. Scope boundary for the MVP: enter-world flow (Login command round trip),
   camera + simple 3D polygon character rendering, right-click context menu
   for available actions, movement input, attack/spell casting against the
   existing combat commands, terrain rendering, and a basic battle loop —
   enough to exercise the already-implemented server-side systems
   end-to-end. Production concerns (asset pipeline, UI polish, full
   accessibility, packaging/distribution) are explicitly out of scope and
   left to a later, separate client decision if this MVP validates the
   direction.

## Resolved questions (2026-07-18)

- **Login**: the MVP client waits for LISS-0147 (session-key login, ADR
  0018) rather than building against the current anonymous claimed-ID
  login. The MVP's Phase 1 (Red) cannot start until LISS-0147 lands on
  `main`.
- **Terrain**: hand-built in Godot (a minimal scene/heightmap authored
  directly in the client). Confirmed that `Field`/`EnvironmentEther`
  (`include/seed/EnvironmentEther.h`) hold no geometric data at all — only
  four scalar floats (Fire/Water/Earth/Air ether levels) used for spell
  power and hazard calculations, no coordinates, heightmap, or mesh data.
  The server has no terrain concept to read from.

## Still open

- Whether this MVP work happens directly against `LISS-0128`/`LISS-0129`
  (updating their status/content) or as new MVP-scoped sub-issues under
  them.

## Consequences

Positive:

- Validates the server-side systems built across this and prior sessions
  end-to-end, with a lightweight engine well-suited to Apple Silicon.
- GDScript avoids adding a second compiled-language toolchain (beyond the
  already-added Kotlin) for a throwaway MVP.

Negative:

- Godot is a fourth major technology surface introduced this session
  (after PostgreSQL, Kotlin/Spring Boot, and React/TypeScript), each with
  its own build/run/debug tooling.
- Re-implementing the binary frame codec in GDScript duplicates logic that
  exists in `network::NetworkFrameCodec` (C++); drift between the two is a
  risk if the wire format changes later without updating both sides.
- This MVP explicitly does not decide the production client technology —
  that remains a separate, larger decision (LISS-0064).

## Related documents

- `docs/issues/LISS-0064-client-shell.md`
- `docs/issues/LISS-0128-client-transport-shell.md`
- `docs/issues/LISS-0129-client-play-shell.md`
- `include/seed/NetworkFrameCodec.h`

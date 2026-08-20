# ADR 0023: Registered Player Authentication — Session-Flow Details

## Status

Accepted — ratified by the Adjudicator on 2026-07-22. The three former Open
Questions are resolved below. `docs/specs/player-authentication-flow-v1.md`
is implementation-ready for the design/test work described by LISS-0146,
LISS-0147, LISS-0148, and LISS-0149.

## Context

ADR 0018 decided the shape of registered player authentication (`users`
table, Postgres-backed `player_sessions`, a new `seed_auth` service,
`seed_server` reduced to session-token validation) but left several
follow-on questions explicitly undecided in its own "Consequences" section:
`seed_auth`'s implementation language, the player-session TTL/reconnect-
window reconciliation, and how a session key crosses from the browser-based
registration/login SPA (LISS-0149) into the native game client. ADR 0019
separately decided `seed_admin`'s language (Kotlin/Spring Boot) but
explicitly did not extend that decision to `seed_auth` ("will likely follow
... though that is not decided by this ADR").

This ADR resolves those specific follow-on questions, based on Adjudicator
direction given on 2026-07-22.

## Decision

### 1. `seed_auth` is Kotlin + Spring Boot, same architecture as `seed_admin`

Extends ADR 0019's stack (Kotlin, Gradle/Kotlin DSL, MyBatis, DDD + Clean
Architecture layering, Docker Compose) to `seed_auth`. `seed_auth` remains
a separate deployed process/instance from `seed_admin` and `seed_server`
(ADR 0018 decision 3 is unchanged) — same language, different service.

### 2. `seed_server` multi-instance deployment is confirmed (not new — restated)

ADR 0018's Context already established this as a constraint shaping the
Postgres-backed `player_sessions` design. This ADR restates it as an
explicit, standalone decision rather than context prose, since it now also
drives the key-rotation design in decision 4 below.

### 3. Game-play login is performed inside the native client

The native game client presents the game-play login form directly and calls
`seed_auth` to obtain a one-time challenge key. Browser deep links, custom URL
schemes, and OS-level callbacks are not used for game-play authentication.
LISS-0149 is limited to account registration, account management, and password
reset; it does not own the game-play login flow.

### 4. Two-stage session keys define the lifecycle

The lifecycle has two keys:

1. `ChallengeKey`: issued by `seed_auth` at the native client's request,
   single-use, and valid for 2 minutes. The client sends it in the login
   request body to `seed_server`.
2. `PlayerSessionKey`: issued by `seed_server` after validating the challenge
   key, valid for 30 minutes, and used for subsequent API communication.

The client refreshes/extends the `PlayerSessionKey` TTL through a client-driven
keep-alive. LISS-0122 reconnect recovery is limited to temporary disconnects
while a valid `PlayerSessionKey` remains available; it is not a separate
five-minute grace period.

### 5. `IdentityAliasStore` deletion is deferred to "after `seed_auth` and `seed_admin` implementations are done," not immediate

ADR 0018 already marked `identity_aliases`/`IdentityAliasStore` as
superseded-not-deleted. This ADR adds a concrete removal trigger: the code
is deleted (not kept dormant indefinitely) once both `seed_auth` (LISS-0146)
and `seed_admin`'s Kotlin migration (ADR 0019/LISS-0144 follow-up) are
implemented and verified. LISS-0150 remains the Issue that executes this.

### 6. No production data exists for `identity_aliases`

Confirmed by the Adjudicator: there is no production deployment yet, so
LISS-0150's data-migration concern is moot. No migration/backfill design is
required for that table.

### 7. Player progression follows a classic MMORPG-style model

LISS-0148 uses the style of a classic, traditional MMORPG. Its schema must
provide independent experience tables for base level and class level; model
equipment as UUID-bearing instances rather than item IDs; store durability,
equipment experience, and socketed-item relationships on equipment instances;
and include character stamina. Experience curves, level limits, item effects,
and other balance parameters are master data editable through `seed_admin`,
separate from player-specific progression state.

### 8. NPC/creature "external injection" is out of scope for this ADR

The Adjudicator noted NPCs and creatures are intended to be "injected from
outside" the same way players are, but the mechanism is undecided and
explicitly deferred; the existing in-game NPC implementation
(LISS-0139 series) is unchanged for now. This is recorded here only so the
note is not lost — it is not a decision this ADR makes, and it has no
effect on LISS-0146/0147/0148/0149/0150.

## Former Open Questions — resolved

1. Game-play login is native-client-only. The client obtains the short-lived
   challenge key from `seed_auth` and sends it in the `seed_server` request
   body; the SPA is not involved.
2. The challenge key lasts 2 minutes. The resulting player session key lasts
   30 minutes and is extended by client keep-alive. LISS-0122 covers recovery
   during a temporary disconnect while that key remains valid.
3. LISS-0148 adopts a classic traditional MMORPG-style progression model with
   the mandatory independent EXP, equipment-instance, socket, stamina, and
   admin-master-data requirements listed in Decision 7.

## Consequences

Positive:

- Resolves ADR 0018's explicitly-deferred follow-on questions (language,
  key lifecycle, and native-client login boundary) without reopening
  ADR 0018's core decisions.
- Keeps `seed_auth`/`seed_admin`/`seed_server` on a consistent two-language
  split (Kotlin for the two backend-for-frontend services, C++ for the
  authoritative world server), matching ADR 0019's stated rationale.

Negative / follow-up work required:

- LISS-0146, LISS-0147, LISS-0148, and LISS-0149 may begin Phase 1 Red design
  and contract tests. Phase 2 remains gated on review of those tests.

## Related documents

- `docs/architecture/adr/0018-registered-player-authentication.md` (base
  decision; this ADR resolves several of its deferred follow-ups)
- `docs/architecture/adr/0019-admin-backend-language-kotlin-spring-boot.md`
  (language/stack extended to `seed_auth` by decision 1)
- `docs/specs/player-authentication-flow-v1.md` (flow diagram derived from
  this ADR)
- `docs/issues/LISS-0146-user-registration-and-auth-service.md`
- `docs/issues/LISS-0147-world-server-session-token-login.md`
- `docs/issues/LISS-0148-player-progression-persistence-schema.md`
- `docs/issues/LISS-0149-registration-login-react-spa.md`
- `docs/issues/LISS-0150-deprecate-anonymous-login-and-alias-reconciliation.md`
- `docs/issues/LISS-0122-reconnect-snapshot-recovery.md`

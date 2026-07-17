# ADR 0018: Registered Player Authentication Replaces Anonymous Alias Login

## Status

Accepted

## Ratification

Ratified by the Adjudicator on 2026-07-18. This ADR supersedes the anonymous
login model (LISS-0123) and the identity-alias reconciliation system built on
top of it (LISS-0130, ADR 0016) for player identity. It does not change
admin authentication (ADR 0017), which remains a separate, already-decided
system.

## Context

The original design assumed players could never be authenticated: connect
with any self-declared nickname, no password. Because two connections
claiming the same nickname could not be told apart from real re-connection
vs. impersonation, LISS-0130/ADR 0016 built a probabilistic "alias
reconciliation" system — confidence scores, a human collision-review queue —
to guess whether a claimed nickname was the same returning player.

The Adjudicator has now decided players will register accounts (username +
password) through a registration page, or be created by an admin, and only
registered users may log in. Once an account is a verified credential, the
entire reconciliation problem disappears: the account **is** the identity,
with no guessing required. Building today's admin authentication (ADR 0017)
first — a real password-based system on the same PostgreSQL database —
directly enabled recognizing this and reusing the same pattern for players.

This decision was also shaped by an existing constraint agreed earlier in
this session: **`seed_server` (the world) is expected to run as multiple
instances**. Any session-validation mechanism must therefore be centrally
shared (PostgreSQL), not per-process in-memory — unlike `AdminSessionStore`
(ADR 0017), which is in-memory because `seed_admin` is assumed single-
instance for now.

## Decision

### 1. `users` table replaces the anonymous-login/alias model

A new PostgreSQL table `users` (same shape as `admin_users`: `id`,
`username` unique, `password_hash` via `pgcrypto`) holds registered player
accounts. `identity_aliases`, `IdentityAliasStore`, and the collision-review
queue (LISS-0130/ADR 0016) are **superseded** — no new work proceeds on
them. They are not deleted (existing local-issue-planning convention);
their issue files are marked superseded, pointing here.

### 2. Player sessions are PostgreSQL-backed, not in-memory

A new `player_sessions` table (`session_token`, `user_id`,
`created_at`, `expires_at`) is the source of truth for "who is this
connection." Because any of the (potentially many) `seed_server` instances
must be able to validate a session token without knowing which instance
originally issued it, the session store cannot be in-memory like
`AdminSessionStore` — it must be a shared table, queried per validation the
same way `PostgresIdentityAliasStore` is queried today.

### 3. A new `seed_auth` service issues sessions; `seed_server` never sees a password

`seed_auth` (new executable, same optional-build pattern as `seed_admin`:
libpqxx + cpp-httplib, skipped when either is absent) exposes:

- `POST /register` — creates a `users` row (`pgcrypto` `crypt()` hash).
- `POST /login` — verifies credentials, mints a `player_sessions` row, and
  returns the session token to the client.
- `POST /logout` — invalidates the session row.

`seed_auth` is a separate process from both `seed_admin` and `seed_server`,
consistent with the Adjudicator's stated principle that the world server and
administrative surfaces stay in separate instances. `seed_server` performs
**no credential verification of its own** — it only ever sees a session
token.

### 4. `seed_server`'s Login command becomes session-token validation

`NetworkCommand::Login`'s payload changes meaning: instead of a free-text
claimed nickname, the client sends the session token obtained from
`seed_auth`. `seed_server` looks the token up in `player_sessions`
(shared PostgreSQL, so any instance sees the same answer), resolves it to a
`user_id`, and treats that `user_id` as the durable player identity for the
connection. The world server does not need to know or reason about
usernames — only the opaque `user_id` it gets back from the lookup.

This directly replaces `SessionRegistry::login(claimedId)`'s current
"accept any well-formed nickname" behavior. The existing internal-id
allocation, active-session tracking, and `isValidClaimedId`/
`canonicalClaimedId` machinery built for the anonymous model no longer
apply to the player login path and will be removed or replaced in the
implementation Issue, not carried forward as dead code.

### 5. Registration/login UI is a React SPA, same technology decision as ADR 0017

Player registration and login use React, matching the technology already
decided for the admin UI (ADR 0017 decision 5) — no new front-end framework
decision is introduced. Whether this is the same deployed app as the admin
UI or a separate build is an implementation-level decision left to the
Issue design phase, not fixed here; the two serve different audiences
(anonymous public registration vs. authenticated admin operators) and
different backends (`seed_auth` vs. `seed_admin`), which is a reason to lean
toward separate apps, but this ADR does not mandate that.

### 6. Player progression data (level, items) also lives in PostgreSQL, keyed by `user_id`

The Adjudicator confirmed player traits/items are stored in the same
PostgreSQL database. Schema for this (inventory, levels, stats) is
explicitly **not** designed by this ADR — it is a separate Issue scoped
after the identity/session model lands, since it depends on `user_id`
existing as the durable key.

## Consequences

Positive:

- Removes an entire class of probabilistic identity-guessing complexity
  (confidence scores, human collision review) that only existed because
  players couldn't be authenticated.
- Reuses the `pgcrypto`-based credential pattern already proven in ADR 0017
  — no new password-hashing dependency.
- `seed_server` is simplified: it becomes a pure session-token consumer,
  never touching credentials, which keeps the world server's trust
  boundary small even as it's horizontally scaled.

Negative / follow-up work required:

- LISS-0123 and LISS-0130 must be formally marked superseded (status
  update, not deletion), and WP-0007's canonical table must drop them from
  the active path.
- `player_sessions` TTL/reconnect-window policy needs to reconcile with the
  already-implemented LISS-0122 (5-minute reconnect Snapshot recovery) —
  the session token's validity window must be at least as long as the
  reconnect window, or reconnection breaks. Not decided by this ADR.
- Whether `seed_auth` and `seed_admin` share a common `PostgresSessionStore`
  component (both need Postgres-backed token issue/validate, just
  `AdminSessionStore` today is in-memory) is an implementation detail left
  open — likely worth unifying to avoid duplicate code, but not decided
  here.
- Exact session token format, length, and player-session TTL are not
  decided here.
- Whether `seed_admin`'s own session store should also move from in-memory
  to `player_sessions`-style PostgreSQL storage (for consistency, or if
  `seed_admin` is ever scaled to multiple instances) is an open question,
  not required now since admin remains single-instance.

## Related documents

- `docs/architecture/adr/0016-identity-alias-persistence-and-review.md`
  (superseded for player identity)
- `docs/architecture/adr/0017-admin-authentication-and-http-surface.md`
  (pattern reused; remains in effect for admin)
- `docs/issues/LISS-0123-anonymous-login-alias.md` (superseded)
- `docs/issues/LISS-0130-identity-persistence-reconciliation.md` (superseded)
- `docs/work-plans/WP-0009-registered-player-authentication.md`

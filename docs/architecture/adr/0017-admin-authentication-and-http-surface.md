# ADR 0017: Admin Authentication and HTTP Admin Surface

## Status

Accepted

## Ratification

Ratified by the Adjudicator on 2026-07-18, closing ADR 0016's admin
authentication ambiguity (decisions 4 and 5: export page, collision review
queue).

## Context

ADR 0016 decided that identity alias export and collision review happen
through an admin-login-gated surface, but explicitly deferred the
authentication mechanism because none existed in this codebase — the only
existing "login" (`SessionRegistry::login`) is an unauthenticated,
self-declared nickname and must never be reused for admin access.

This project also has no HTTP server today; `ServerRuntime`/`seed_server`
speaks a custom binary TCP protocol (`NetworkFrameCodec`) to game clients
only. Building the admin export/review surface requires introducing an HTTP
stack for the first time, which is new architectural ground beyond ADR 0016's
scope.

## Dependency Adoption Evidence (cpp-httplib)

- **Security posture**: `cpp-httplib` 0.50.1 (Homebrew bottle, MIT license).
  No specific CVE check was run against this exact version in this session;
  this must be re-checked against the pinned version before any
  internet-facing deployment (see Consequences).
- **Version-specific examples**: header-only, single-header library;
  `find_package(httplib CONFIG)` provides an `httplib::httplib` CMake target
  in the installed package, used directly.
- **Troubleshooting depth**: widely used, single-file library with an
  extensive README and long-lived GitHub project; low integration risk.
- **Minimal real-file test**: the admin HTTP surface is exercised with a
  manual `curl`-based connection test (mirroring the LISS-0056 manual
  connection test pattern), not an automated integration test, consistent
  with Adjudicator policy deferring `ctest` execution this session.
- **POC feasibility**: this ADR's implementation slice (LISS-0144) is itself
  the POC — a minimal login + export + review-queue HTTP surface.
- **Boundary fit**: only the new `seed_admin` executable and its adapter
  sources include `httplib.h`. `seed_core` and `seed_postgres` remain free of
  any HTTP dependency; the admin surface reuses the existing
  `session::IdentityAliasStore` port for review/export instead of
  duplicating alias logic.

No JSON library is adopted. Request bodies use
`application/x-www-form-urlencoded` (parsed natively by `cpp-httplib`);
response bodies are small, fixed-shape JSON arrays hand-serialized with an
escaping helper, avoiding a second new dependency for this minimal surface.

## Decision

### 1. Admin identity is independent of player identity

A new PostgreSQL table `admin_users` (separate from `identity_aliases`)
holds admin accounts: `id`, `username` (unique), `password_hash`. It lives in
the same `seed_identity` database as `identity_aliases` for this MVP slice —
not a separate database or schema. Splitting it out is a future hardening
step, not required now.

### 2. Password hashing uses PostgreSQL's `pgcrypto`, not a new C++ dependency

Passwords are hashed with `pgcrypto`'s `crypt(password, gen_salt('bf'))`
(bcrypt) at write time, and verified with
`password_hash = crypt(candidate, password_hash)` at read time. This keeps
credential handling inside the already-adopted PostgreSQL dependency instead
of adding a C++ password-hashing library (libsodium, bcrypt bindings, etc.).
`AdminAuthStore` (in the `seed_postgres` adapter, alongside
`PostgresIdentityAliasStore`) is the only code that touches this table.

### 3. Session tokens are server-side, in-memory, and Bearer-only (no cookies)

`AdminSessionStore` (dependency-free, lives in `seed_core` next to
`SessionRegistry`) issues an opaque token on successful login, keyed to an
admin user id with an expiry. Tokens are 32 bytes read from `/dev/urandom`,
hex-encoded. Clients send `Authorization: Bearer <token>` on every
subsequent request; there is no cookie, so there is no CSRF surface to
defend — a browser cannot be tricked into attaching a custom `Authorization`
header via a cross-site form or image tag the way it can with cookies.

**Ambiguity carried forward**: token TTL, revocation-on-idle, and
multi-instance session sharing (if the admin surface is ever
horizontally scaled) are not decided here; the implementation slice uses a
fixed TTL as a placeholder, not a policy decision.

### 4. HTTP library is `cpp-httplib`

Chosen over Crow/Pistache/Boost.Beast for this MVP because it is
header-only, has no transitive runtime dependency beyond OpenSSL (only
needed for HTTPS, not used yet), and keeps the admin surface's footprint
proportional to its scope (a handful of JSON/form endpoints, not a general
web framework).

### 5. Scope: JSON/form HTTP API only, no browser UI

This ADR and its implementation slice (LISS-0144) deliver a `curl`-testable
HTTP API (`seed_admin` executable): login, export, review-queue listing, and
review submission. **A browser-based admin UI is explicitly out of scope**
here — that requires its own front-end technology decision (still an open
Non-Decision per `CLAUDE.md`) and is left to a future Issue. "Admin page" in
ADR 0016 is satisfied at the API level for this slice; a UI can be layered
on top later without changing this contract.

### 6. Admin account bootstrap is a manual DB operation, not an API endpoint

There is no "create admin" HTTP endpoint — that would be an unauthenticated
privilege-escalation surface. The first admin account is inserted directly
via `psql` using `pgcrypto`'s `crypt()`, documented in `db/README.md`. This
is acceptable for the current single-operator development stage; a
self-service admin-invite flow is future work if the operator model changes.

## Consequences

Positive:

- Admin credentials are fully separated from the anonymous player identity
  model; no path exists for a player to escalate to admin via claimed ID.
  A stronger form of this positive outcome — an explicit ownership-escape
  audit — is already covered by the prior safety-gate work in LISS-0019 and
  the LISS-0131 rate-limit slice; this ADR extends the same "no
  self-declared privilege" posture to the new admin surface.
- No CSRF defense code is needed given the Bearer-only design.
- No new C++ password-hashing dependency; `pgcrypto` is server-side and
  already covered by the PostgreSQL adoption in ADR 0016.

Negative / follow-up work required:

- `cpp-httplib`'s specific pinned version has not had a full vulnerability
  advisory check in this session; required before any non-local deployment.
- No rate limiting on `/login` yet — a brute-force guard (reusing the
  `CommandRateLimiter` pattern from LISS-0131) is a follow-up, tracked as a
  remaining decision below.
- No HTTPS/TLS termination is configured; the admin surface is
  local/loopback-only until a deployment ADR decides TLS termination
  (reverse proxy vs. in-process).
- Token revocation-on-idle, TTL policy, and multi-instance session sharing
  remain open (see Decision 3 ambiguity).
- A browser UI remains a separate, undecided Non-Decision (Decision 5).

## Related documents

- `docs/architecture/adr/0016-identity-alias-persistence-and-review.md`
- `docs/issues/LISS-0130-identity-persistence-reconciliation.md`
- `docs/issues/LISS-0143-postgres-identity-alias-adapter.md`
- `docs/issues/LISS-0144-admin-authentication-http-surface.md`
- `include/seed/PostgresIdentityAliasStore.h`

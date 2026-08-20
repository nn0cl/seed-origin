# ADR 0016: Identity Alias Persistence, Retention, Review, and Encryption Scope

## Status

Superseded by ADR 0018 for player identity (formalized by LISS-0150,
2026-08-20). Kept as a historical record; no new work proceeds from this ADR.

## Ratification

Ratified by the Adjudicator on 2026-07-18 as the closing decision set for
LISS-0130's "Remaining decisions." This ADR records the decision; the
PostgreSQL adapter, retention/purge job, admin export page, and collision
review queue are separate implementation Issues gated behind their own
Phase 1/2/3 cycles.

## Context

LISS-0130 introduced `IdentityAliasStore` as a port and shipped
`InMemoryIdentityAliasStore` as the only adapter, deliberately deferring:

- the real persistence engine and schema.
- retention period and backup policy.
- export mechanism and its authorization.
- how a human resolves a claimed-ID collision or low-confidence
  reconciliation.
- whether/how the claimed ID needs encryption at rest.

`CLAUDE.md`'s "Current Non-Decisions" lists "Identity persistence
format/backend" as an explicit ADR topic (tracked by LISS-0066/0068). This
ADR closes that topic for the identity-alias slice specifically; it does not
decide server deployment/monitoring platform or any other Non-Decision.

## Dependency Adoption Evidence (PostgreSQL)

PostgreSQL is a new runtime dependency and goes through
`docs/architecture/dependency-policy.md`'s adoption checklist. The
Adjudicator has selected PostgreSQL as the engine; the checklist below
records what is and is not yet verified. Items marked "pending" must be
closed before the implementation Issue reaches Phase 2 Green, not before
this ADR is accepted — the engine choice itself is not blocked on them.

- **Security posture**: pending. No specific PostgreSQL version or client
  driver has been chosen yet; a vulnerability/advisory check must be run
  against the concrete version pinned in the implementation Issue.
- **Version-specific examples**: pending, same reason.
- **Troubleshooting depth**: not evaluated yet; PostgreSQL itself is a
  mature, well-documented dependency, but the specific C++ client library
  (driver) has not been selected.
- **Minimal real-file test**: not applicable until the adapter Issue starts;
  the port (`IdentityAliasStore`) is already exercised against a real
  in-memory implementation, so the interface contract itself is proven.
- **POC feasibility**: required before Phase 2 of the PostgreSQL adapter
  Issue, per dependency-policy's "affects architecture, external I/O,
  persistence" trigger.
- **Boundary fit**: PostgreSQL access must live entirely inside a new
  adapter class implementing `session::IdentityAliasStore`
  (`include/seed/IdentityAliasStore.h`). No SQL, connection handling, or
  schema knowledge may appear in `SessionRegistry` or any domain/use-case
  code — this is already enforced by the existing port boundary and does
  not change.

A driver/client-library choice, connection pooling, and migration tooling
are explicitly **not** decided by this ADR and remain open ambiguities for
the implementation Issue.

## Decision

### 1. Persistence engine

The identity alias store's real backend is **PostgreSQL**, accessed only
through a new adapter implementing `session::IdentityAliasStore`. The
in-memory adapter remains the default for tests and for any environment
without a configured PostgreSQL connection; it is not deleted by this ADR.

Schema design, migration tooling, and connection-pooling strategy are
deferred to the implementation Issue and are not decided here.

### 2. Retention period

Retention is **configurable**, with a **default of 2 years (730 days)**.
The retention window is measured from each record's `lastUsedTick`-derived
activity, not `createdTick`, so an alias that is still in active use is
never purged solely for being old.

**Ambiguity carried forward**: `lastUsedTick` is a deterministic
`WorldTick`, not a wall-clock timestamp, and world ticks do not have a
fixed real-time duration guaranteed across all deployments. A retention
policy expressed in calendar days requires a wall-clock timestamp
association that does not exist in the current `IdentityAliasRecord` shape.
The implementation Issue must decide how activity ticks map to wall-clock
time (e.g. store a wall-clock `lastUsedAt` alongside `lastUsedTick`) before
a purge job can be built. This ADR fixes the *policy* (configurable, default
2 years); it does not fix the *mechanism* for measuring elapsed real time.

### 3. Claimed ID content scope

The claimed ID is a **nickname only**. It must never be used to collect or
imply contact information (email address, phone number, physical address).
This is a product/privacy constraint on what the login flow may prompt for
and accept, not only a storage constraint.

The existing `SessionRegistry::isValidClaimedId` character set
(alphanumeric, `_`, `-`, `.`, max 64 characters) already rejects `@` and
most punctuation needed to express an email address or phone number, so no
code change is required by this decision — this section records the intent
so a future change to the validation rule cannot silently start accepting
contact-shaped identifiers without revisiting this ADR.

### 4. Export mechanism

Export is performed through a **dedicated admin page that requires
login**. `SessionRegistry::exportAliasRecords()` already exists as the
use-case-level read path; the admin page is a new adapter/delivery surface
that calls it, not a change to the export use case itself.

**Ambiguity carried forward**: no admin authentication mechanism exists in
this codebase. `CLAUDE.md`'s Project Boundaries state "the unauthenticated
login is a temporary session, not an authenticated identity" — the current
`SessionRegistry` has no concept of a privileged/admin identity at all. This
ADR decides *that* export requires an authenticated admin surface; it does
not decide the admin authentication mechanism (password, SSO, local
allowlist, etc.). That is a separate Architecture Path decision and must
precede any admin-page implementation Issue reaching Phase 2.

### 5. Collision review UI

The collision/low-confidence review workflow is a **queue-type review
screen** (option 1 of the three presented): records requiring human
judgment are surfaced in a queue, and an admin resolves each one with
Confirm or Reject. This calls `SessionRegistry::recordAliasReview` (added
in this LISS-0130 slice) as the terminal action.

Queueing criteria (confidence threshold, anomalous re-login frequency, or
other triggers) are **not** decided by this ADR and are left to the
implementation Issue. No comparison-UI (option 2) or self-report/dispute
flow (option 3) is adopted at this time; either may be proposed later as a
separate ADR if the queue-only flow proves insufficient.

### 6. Encryption scope

Because the claimed ID is nickname-only and contact information is
explicitly excluded (Decision 3), the data sensitivity of the stored
claimed ID is judged **low relative to PII such as email or phone**. The
selected control level is:

- **At-rest encryption via the PostgreSQL host/storage layer** (managed
  disk encryption or `pgcrypto` column encryption), not application-layer
  hashing or a dedicated key-management port.
- **Access control and audit logging** on the admin export page and the
  collision review queue are the primary safeguard for who can view
  claimed IDs, not cryptography.
- Application-layer encryption/hashing (the "hash for search, encrypt raw
  value" pattern) is explicitly **not** adopted now. It remains an option
  to revisit if the claimed-ID content policy ever changes to allow
  contact-shaped identifiers, since that would raise the sensitivity tier
  and justify the added key-management complexity (`AGENTS.md`'s "Secret
  storage" port).

## Consequences

Positive:

- The three deferred product/privacy questions (retention, export, review)
  now have a recorded decision instead of remaining open indefinitely.
- The nickname-only constraint keeps the encryption/PII surface small and
  avoids introducing a Secret Storage port prematurely.
- The queue-type review UI reuses the already-implemented
  `recordAliasReview` use case with no further domain changes.

Negative / follow-up work required before implementation:

- A wall-clock-to-WorldTick mapping must be designed before any retention
  purge job can run (see Decision 2 ambiguity).
- An admin authentication mechanism does not yet exist and must be decided
  separately before the export page or review queue can be built (see
  Decision 4 ambiguity).
- PostgreSQL driver selection and its full dependency-adoption checklist
  remain open (see Dependency Adoption Evidence).
- `CLAUDE.md`'s "Current Non-Decisions" list still reads "Identity
  persistence format/backend" as fully open; it should be updated to
  reflect that the engine is decided (PostgreSQL) while schema/migration
  remain open. That edit is an agent-operating-contract change and must go
  through `docs/collaboration/prompt-instruction-change-control.md` (trace
  file + explicit Adjudicator review) as its own change, not bundled into
  this ADR.

## Related documents

- `docs/issues/LISS-0130-identity-persistence-reconciliation.md`
- `docs/architecture/dependency-policy.md`
- `docs/collaboration/prompt-instruction-change-control.md`
- `include/seed/IdentityAliasStore.h`
- `include/seed/SessionRegistry.h`

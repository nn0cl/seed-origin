# Trace: LISS-0152 completion handoff (CSP + reconnect contract)

- Date: 2026-08-20
- Path: Feature Path (documentation + status sync; no code change)
- Issue: LISS-0152
- Branch: `feature/liss-0152-client-side-prediction`
- Worktree: `/Users/nn0cl/Documents/git/seed-origin-prediction`
- Phase: Phase 2 Green complete → `review` pending Adjudicator sign-off
- Planning record: AIP-0152-001

## Current State

- **Current phase:** Phase 2 Green complete; awaiting Adjudicator review for
  `review` → `done`.
- **User request:** Continue the next safe slice on the CSP branch without
  touching auth, LISS-0155, or merge/push.
- **Scope:** Local-player client-side prediction, server reconciliation,
  full-vs-delta WorldUpdate contract, Disconnect lifecycle, reconnect
  RequestSnapshot I/O (LISS-0128 slice on this branch), temporary Login Field
  placement (LISS-0153 early subset).
- **Out of scope:** seed-auth, PlayerName case/Unicode fold, zone spawn,
  auth-side name ledger, renderer/UI (LISS-0064), merge/push.

## Completed

### Artifacts (code, prior slices on this branch)

| Area | Key modules / tests |
| --- | --- |
| Sequenced Move + server ack | `MovementIntent`, `WorldFrameApplier`, owner-only ack personalization |
| Client prediction + replay | `LocalMovementPredictor`, `PendingInputBuffer`, `ClientPredictionSyncTest` |
| Full vs delta WorldUpdate | Snapshot/Event one sequence column; gap → RequestSnapshot |
| RequestSnapshot wire (LISS-0154) | Command type 7, empty payload, coalesced Snapshot per tick |
| Reconnect I/O (LISS-0128 slice) | `ClientTransportShell`, `ClientInboundDemux`, loopback + socketpair tests |
| Disconnect lifecycle | `DisconnectCommandHandler`, Accepted ack → auth reset + TCP close |
| Login Field placement (early) | `FieldSessionPresence`, claimed-name registry, join Snapshot |
| Remote pose (non-predicted) | `RemotePlayerPoseStore`, gameplay-id keying |
| Production server tick loop | `ServerMain.cpp` publishes snapshots + owner acks on 20 Hz tick |

### Decisions captured

- Owner-only movement ack on the same WorldUpdate sequence (2026-08-17).
- Full Snapshot vs delta Event share one sequence column (2026-08-19).
- Disconnect: Accepted ack → flush → TCP close; Rejected keeps socket (2026-08-20).
- Reconnect: new TCP, re-Login, auto RequestSnapshot when `needsSnapshot` (spec).

### Verification (2026-08-20)

```text
cd /Users/nn0cl/Documents/git/seed-origin-prediction
./build/seed_tests   # EXIT 0 (full suite)
```

Prior note about `queues_attack_and_spell_intents` abort is **resolved** on
this branch; full suite is green.

## Reconnect path audit (spec vs implementation)

Spec (`client-side-prediction-v1.md` §Login Field placement):

1. Disconnect ack Accepted → client resets transport auth, closes TCP.
2. Reconnect uses new TCP (`beginReconnect` / `connectTcp`).
3. After Login → RequestSnapshot Command on socket.
4. Server coalesces → one Snapshot on shared sequence.
5. Client clears `snapshotRequested`, resumes Events.

| Step | Evidence |
| --- | --- |
| Accepted Disconnect closes TCP | `ClientTransportShellTest::rejected_disconnect_keeps_tcp_accepted_ack_closes_it` |
| Server flushes ack then closes | `DisconnectCommandHandlerTest`, `ServerRuntimeTest` |
| `beginReconnect` sets `needsSnapshot` | `ClientWorldUpdateReceiver::beginReconnect`, transport tests |
| Login → RequestSnapshot on wire | `after_reconnect_and_login_writes_request_snapshot_on_the_socket` |
| Loopback E2E Snapshot applied | `loopback_reconnect_sends_request_snapshot_and_applies_server_snapshot` |
| Disconnect → rebind same gameplay id | `loopback_disconnect_ends_session_and_resets_client_auth` |

**Gap note (non-blocking):** `loopback_disconnect_ends_session_and_resets_client_auth`
does not assert RequestSnapshot on the post-disconnect reconnect path; that
behavior is covered by the socketpair and separate loopback reconnect tests.
No code change required for this handoff slice.

## Changed Files (this slice)

- `docs/collaboration/traces/2026-08-20-liss-0152-completion-handoff.md` (this file)
- `docs/issues/LISS-0152-client-side-prediction-reconciliation.md` (status + verification)
- `docs/work-plans/WP-0010-client-side-prediction.md` (LISS-0152 row + next issue)

## Context Ledger

- **Included:** CSP acceptance criteria, reconnect/disconnect contract, DoD
  issue-status sync, full `seed_tests` verification.
- **Omitted:** seed-auth, LISS-0155, zone spawn, name normalization, merge/push.
- **Assumptions:** Temporary origin `(0,0,0)` and in-memory claimed-name
  registry remain until LISS-0153 Adjudicator decisions land.
- **Open decisions (Adjudicator):**
  - LISS-0153: case/Unicode name fold, zone spawn, auth-side name ledger,
    operator UI/permissions.
  - LISS-0128 remainder: timeout, backoff, ops/UI.
  - Whether LISS-0152 moves to `done` now or waits for LISS-0153 slice on
    the same branch.

## Next Safe Action

1. Adjudicator review LISS-0152 for `review` → `done` (or request Phase 3
   refactor on specific modules).
2. If approved, next implementation slice is LISS-0153 ambiguities **only
   after** Adjudicator selects normalization/spawn/ledger policy.
3. LISS-0128 remainder (timeout/ops) stays separate from CSP core.
4. Do **not** merge/push without explicit Adjudicator confirmation.

## Blockers

- None for documentation/handoff. LISS-0153 durable spawn and name-policy
  items are intentionally deferred pending Adjudicator decision.

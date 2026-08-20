# Trace: LISS-0128 completion handoff (timeout / ops Phase 3)

- Date: 2026-08-20
- Path: Feature Path AT-TDD Phase 3 Refactor
- Issue: LISS-0128
- Branch: `feature/liss-0128-reconnect-timeout-ops`
- Worktree: `/Users/nn0cl/Documents/git/seed-origin-prediction`
- Phase: Phase 3 Refactor complete → **done**
- Planning records: AIP-0128-001, AIP-0128-002

## Phase 3 Refactor (2026-08-20)

| Item | Change |
| --- | --- |
| Connect timeout | `connectTimeoutMs` + non-blocking connect / poll with monotonic deadline |
| Invalid frame detection | 2-byte prefix heuristic moved into `ClientInboundDemux` |
| `lastErrorDetail` | `formatTransportErrorDetail` for consistent reason strings |
| Deadline extraction | `TransportWaitDeadline` for login / snapshot waits |
| Error responsibility | `recordError` (state only) vs `markFailed` (Failed + close + clear) |

Verification:

```text
cd /Users/nn0cl/Documents/git/seed-origin-prediction/build
cmake --build . && ./seed_tests   # EXIT 0
```

## Current State

- **Current phase:** **done** (LISS-0128 timeout/ops slice complete).
- **Scope:** monotonic transport deadlines, error state machine, ops counters,
  reconnect RequestSnapshot I/O (prior slices).
- **Out of scope:** exponential backoff, UDP, Godot/UE UI (LISS-0129, LISS-0064),
  LISS-0132 observability, seed-auth.

## Completed Artifacts

- `TransportWaitDeadline`, `TransportErrorDetail`, `TransportTimeouts.connectTimeoutMs`
- `ClientInboundDemux` prefix validation
- `ClientTransportShell` connect timeout + refactor
- Tests: detail assertions, connect timeout, demux prefix cases
- Issue LISS-0128 → `done`

## Next Safe Action

- Merge PR into `main` after Adjudicator review.
- Follow-on: LISS-0132 observability, remaining LISS-0128 backlog (backoff/UI).

## Blockers

- None.

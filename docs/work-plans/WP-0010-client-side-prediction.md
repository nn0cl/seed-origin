# Work Plan: Local-player client-side prediction

## Goal

Introduce client-side prediction and server reconciliation for local player
movement without weakening the authoritative 20 Hz simulation.

## Scope

- In: sequenced Move payload, server admission/ack, pending input replay,
  prediction vs render smoothing, snapshot rebase.
- Out: remote entity prediction, combat prediction, renderer, auth work.

## Issue Graph

| Issue | Status | Initial size | Current size | Planning record | Depends on | Blocks | Branch |
| --- | --- | --- | --- | --- | --- | --- | --- |
| LISS-0152 | in_progress | L | L | AIP-0152-001 | LISS-0121, LISS-0125 | LISS-0153 | feature/liss-0152-client-side-prediction |
| LISS-0153 | in_progress | M | M | AIP-0153-001 | LISS-0152 | - | feature/liss-0152-client-side-prediction |
| LISS-0154 | review | M | M | AIP-0154-001 | LISS-0121, LISS-0122, LISS-0152 | LISS-0128 | feature/liss-0152-client-side-prediction |
| LISS-0128 | in_progress | L | M (I/O slice) | AIP-0128-001 | LISS-0121, LISS-0122, LISS-0154 | LISS-0129 | feature/liss-0152-client-side-prediction |

## Recommended Order

1. LISS-0152 (temporary origin Field placement on Login)
2. LISS-0154 (RequestSnapshot wire Command)
3. LISS-0128 reconnect I/O slice (RequestSnapshot on POSIX TCP)
4. LISS-0153 (durable spawn policy)

## Current Next Issue

- Issue: remaining LISS-0153 ambiguities (name normalization, rename,
  empty-name publication, auth-side name ledger, zone spawn) or remaining
  LISS-0128 (timeout/ops/UI)
- Reason the I/O slice is unblocked: LISS-0154 RequestSnapshot is on this
  branch and the reconnect socket path now sends that Command after Login.
- Adjudicator approval (2026-08-17): owner-only movementAck; protocol version
  stays 1; public movement remains the existing `movement=` broadcast.
- Adjudicator approval (2026-08-19): full Snapshot and delta Event share
  the same WorldUpdate sequence (not a second channel). Snapshot replaces
  all public poses including idle players (join / gap / reconnect). 20 Hz
  Events publish movers only (`;x=;y=;z=` on non-owner copies). Gaps
  trigger another full fetch, not Event inference. Owner ack stays on the
  owner's copy of that sequence. 20 Hz is tick + delta cadence, not a
  full-Snapshot rate. RequestSnapshot is LISS-0154 (type 7, empty
  payload, at most one Snapshot per tick). Login Field placement uses
  LISS-0153 (configurable spawn, four identity roles, unique PlayerName).
  Observers key remotes by gameplay id; session is communication; HUD uses
  PlayerName.
  No new ADR: operational confirmation of the existing sequence column.

## Risks

- Mixing WorldUpdate sequence with client input ack.
- Treating Snapshot and Event as separate sequence channels, or emitting
  a full Snapshot every 20 Hz tick.
- Divergent client/server integration rules.
- Accidentally editing in-flight auth files on the original worktree.

## Verification Plan

- New Gherkin-mapped CTest cases in `seed_tests`.
- Existing movement, WorldUpdate, and receiver tests remain green.

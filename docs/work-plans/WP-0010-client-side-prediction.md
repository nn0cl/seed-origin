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
| LISS-0152 | in_progress | L | L | AIP-0152-001 | LISS-0121, LISS-0125 | - | feature/liss-0152-client-side-prediction |

## Recommended Order

1. LISS-0152

## Current Next Issue

- Issue: LISS-0152
- Reason it is unblocked: LISS-0121 and LISS-0125 already landed on main.
- Adjudicator approval (2026-08-17): owner-only movementAck; protocol version
  stays 1; public movement remains the existing `movement=` broadcast.

## Risks

- Mixing WorldUpdate sequence with client input ack.
- Divergent client/server integration rules.
- Accidentally editing in-flight auth files on the original worktree.

## Verification Plan

- New Gherkin-mapped CTest cases in `seed_tests`.
- Existing movement, WorldUpdate, and receiver tests remain green.

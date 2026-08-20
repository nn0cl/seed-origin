# Trace: First Event sequence gap (LISS-0152 slice)

- Date: 2026-08-20
- Path: Feature Path, Phase 2 Green (bug fix)
- Issue: LISS-0152 (touches LISS-0121 gap contract)
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Fix `rejects_sequence_gap_without_partial_application` (`assert(!receive)` SIGABRT).
Do not change Then clauses. Do not push.

## Included

- `WorldUpdateSequenceTracker` first-packet policy
- `ClientWorldUpdateReceiver` / hazard ingest path
- network-protocol-v1 and CSP Gherkin: Event gap → RequestSnapshot

## Omitted

- seed-auth, LISS-0146–0150, LISS-0155
- Next full-suite failure (`rejects_unimplemented_command` / Move dispatch)

## Routing

Composer locally; `./build/seed_tests` in this worktree. No LLM game state.

## Cause

Uninitialized tracker accepted any first WorldUpdate, including Event
sequence 5 while expected was 1. Snapshot bootstrap at a non-1 sequence
remains valid (join). Events must match expected 1 or be a gap.

## Verification

`./build/seed_tests`: gap receiver test passes. Next abort is
`ServerCommandDispatcherTest.cpp:26` `rejects_unimplemented_command`.

# Trace: Trim-empty names and claimed-name registry (LISS-0153)

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0153 (parent LISS-0152)
- Planning record: AIP-0153-004
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Treat whitespace-only PlayerName as empty after ASCII trim. Keep uniqueness
after logout and Field unset via an in-memory claimed-name registry. Do not
invent Unicode or case folding. Do not touch seed-auth. Do not push.
Protocol v1.

## Included

- `trimPlayerName` for empty checks and uniqueness comparison
- claimed names keyed by auth PlayerId, surviving logout/`unsetPlayer`
- acceptance tests for whitespace-only, post-unset collision, trim collision

## Omitted

- seed-auth / LISS-0146–0150 persistence ledger
- Operator UI and permission model
- Unicode / case-fold normalization
- Zone spawn

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

## Adjudicator decisions

Whitespace-only names are empty. Uniqueness is trim then exact match and
survives Field unset. Auth ledger remains later work.

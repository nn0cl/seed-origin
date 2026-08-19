# Trace: Login spawn policy and identity split (LISS-0153)

- Date: 2026-08-19
- Path: Feature Path
- Issue: LISS-0153 (parent LISS-0152)
- Planning record: AIP-0153-001
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Configurable Login spawn, four identity roles, unique PlayerName. Do not
touch seed-auth. Do not push. Protocol v1. No 20 Hz full Snapshot channel.

## Included

- `LoginFieldSpawnSettings` / policy, early defaults (0,0,0) 10/10, max 1024
- `AuthenticatedPlayerIdPort` stub
- Field session↔gameplay binding, gameplay id as existing `targetId` key
- Snapshot `session`, `id`, `name`; auth id omitted
- Unique exact-match PlayerName on Field residents

## Omitted

- seed-auth / LISS-0146–0150 implementation
- Name normalization, rename, auth-side name ledger
- Public stable UUID

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

## Adjudicator decisions

Session updates on reconnect; auth PlayerId is private and stable; gameplay
id is for targeting; PlayerName is display-only and unique (exact match).

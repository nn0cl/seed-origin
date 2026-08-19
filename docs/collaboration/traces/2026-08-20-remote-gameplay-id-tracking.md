# Trace: Remote tracking by gameplay id (LISS-0153)

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0153 (parent LISS-0152)
- Planning record: AIP-0153-002
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Apply the decided identity contract on the client: other people are tracked
by gameplay id after reconnect. HUD uses PlayerName. Do not touch auth.
Do not push. Protocol v1. Skip zone spawn, rename, case normalization.

## Included

- `RemotePlayerPoseStore` keyed by Snapshot `player.<i>.id`
- session kept as a communication field for `movement=` lookup
- display name on the remote pose
- Snapshot applier rejects `player.<i>.authPlayerId`
- receiver tests for reconnect without duplicating remotes

## Omitted

- seed-auth / LISS-0146–0150
- Name normalization, rename, empty-name policy, auth-side ledger
- Zone spawn
- Combat MP 5 vs 4

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

## Adjudicator decisions

Gameplay id is the remote store key. Session may change on reconnect
without creating a second remote. Auth PlayerId is not on the public
Snapshot. HUD reads PlayerName only.

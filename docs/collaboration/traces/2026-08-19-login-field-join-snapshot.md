# Trace: Login Field placement for join Snapshot

- Date: 2026-08-19
- Path: Feature Path
- Issue: LISS-0152 (temporary placement); LISS-0153 opened for durable spawn
- Planning records: AIP-0152-001, AIP-0153-001
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Join Snapshot after Login had `player.count=0` because Login bound a session
without `Field::setPlayer`. Do not invent durable spawn. Do not add
RequestSnapshot Command or 20 Hz full Snapshots. Do not touch auth.

## Included

- Login bind, Field `setPlayer`/`unsetPlayer`, `publicPlayerPoses`, join
  Snapshot via `capturePublicSnapshotIfNewSessions`.

## Omitted

- seed-auth / LISS-0146–0150
- RequestSnapshot wire Command
- Durable spawn table (zone, HP/MP, reconnect restore)

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

## Result

Temporary origin placement (`PlayerId == session.internalId`, `(0,0,0)`,
`Status()`). Tests in `FieldSessionPresenceTest.cpp`.

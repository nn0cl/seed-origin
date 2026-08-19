# Trace: RequestSnapshot wire Command (LISS-0154)

- Date: 2026-08-19
- Path: Feature Path
- Issue: LISS-0154 (parent LISS-0122)
- Planning record: AIP-0154-001
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Wire the internal RequestSnapshot flag as a protocol v1 Command. Fan out at
most one existing public Snapshot per tick. Do not touch auth. Do not push.

## Included

- CommandType 7, empty payload, session from connection
- Dispatcher + per-tick coalesce with join Snapshot
- Client command builder after gap / beginReconnect
- Skip Events while waiting so a later Snapshot in the same buffer applies

## Omitted

- seed-auth / LISS-0146–0150
- 20 Hz full Snapshots
- LISS-0153 spawn
- Reconnect TCP I/O (LISS-0128)
- UDP, AoI, separate sequence

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

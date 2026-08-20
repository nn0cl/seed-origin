# Trace: Operator-only PlayerName and immutable ids (LISS-0153)

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0153 (parent LISS-0152)
- Planning record: AIP-0153-003
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Apply Adjudicator identity rules: empty names are invalid for placement
and Snapshot publication; names are operator-assigned only; auth PlayerId
and gameplay id stay stable across reconnect; session updates. Do not
touch auth. Do not push. Protocol v1. Skip zone spawn and name
normalization.

## Included

- `FieldSessionPresence::operatorSetPlayerName` and always-failing
  `playerSetPlayerName`
- Login placement no longer copies `claimedId` into PlayerName
- Snapshot builder/applier require a non-empty `player.<i>.name`
- Auth PlayerId assignment is one-shot on `Player`

## Omitted

- seed-auth / LISS-0146–0150
- Operator UI and permission model
- Name case/whitespace normalization
- Zone spawn

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

## Adjudicator decisions

Empty names are forbidden. Players cannot rename. claimedId is login-only.
Gameplay id and auth PlayerId do not change on reconnect; session does.

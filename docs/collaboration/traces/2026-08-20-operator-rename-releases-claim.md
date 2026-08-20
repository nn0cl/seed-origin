# Trace: Operator rename releases previous claimed name (LISS-0153)

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0153 (parent LISS-0152)
- Planning record: AIP-0153-005
- Branch: `feature/liss-0152-client-side-prediction`
- Phase: AT-TDD Red then Green in one Adjudicator-requested slice

## Request

When an operator assigns a new unique PlayerName, unclaim the previous
trimmed name and monopolize the new one. Players still cannot rename.
Empty/whitespace-only names stay rejected. Uniqueness is trim-then-exact
match. IDs stay immutable. Do not touch LISS-0155, seed-auth, or push.

## Included

- claimed-name registry keyed by auth PlayerId
- `FieldSessionPresence::operatorSetPlayerName`
- LISS-0153 / CSP spec notes and Gherkin
- presence acceptance tests

## Omitted

- Unicode / case-fold normalization
- zone / logout-location spawn
- Combat MP 5 vs 4
- seed-auth persistence ledger
- LISS-0155 workspace I/O guard worktree
- uncommitted auth files in `seed-origin`

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

## Adjudicator decisions

Operator rename releases the previous claim. Logout still does not
release a current claim. Player rename remains impossible.

## Verification

`seed_tests` in `seed-origin-prediction/build` ran the new FieldSessionPresence
cases (they execute before combat resource tests). The suite later aborted on
the pre-existing Combat MP 5 vs 4 assert, which this slice did not change.

## Open decisions

- PlayerName case fold / Unicode
- Zone / logout-location spawn
- Auth-side name ledger
- Operator UI and permission model

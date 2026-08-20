# Trace: LISS-0147 dispatcher challenge Login slice

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0147
- Branch: `feature/liss-0147-dispatcher-challenge-login`
- Base: main `6756355` (PR #5 Login wire)

## Adjudicator Approval

- Phase 1 Red / Phase 2 Green / Phase 3 Refactor approved 2026-08-20.
- Scope: `ServerCommandDispatcher` constructor that routes Login through
  `ChallengeLoginCommandHandler` when challenge auth is bound.
- Out of scope: ServerRuntime production bind, Postgres adapters, deleting
  anonymous Login path.

## Commits

- `7224f6d` Phase 1 Red
- `7a4ed50` Phase 2 Green
- `38f0366` Phase 3 Refactor

## Verification

```text
cmake --build build --target seed_tests && ./build/seed_tests  # EXIT 0
```

## Next safe action

- Merge after CI.
- Then ServerRuntime bind Phase 1 Red, or Postgres adapter planning.

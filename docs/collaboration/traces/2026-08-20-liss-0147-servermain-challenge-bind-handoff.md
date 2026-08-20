# Trace: LISS-0147 ServerMain bootstrap bind slice

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0147
- Branch: `feature/liss-0147-servermain-challenge-bind`
- Base: main `c58b9f6` (PR #7 runtime merge)

## Adjudicator Approval

- Phase 1 Red / Phase 2 Green / Phase 3 Refactor approved 2026-08-20.
- Scope: `ServerBootstrap` factory; `ServerMain` uses production bootstrap;
  `SEED_CHALLENGE_AUTH` fails fast until Postgres challenge adapters exist.
- Out of scope: Postgres adapters, full challenge wiring when env is set,
  anonymous login removal.

## Verification

```text
cmake --build build --target seed_tests seed_server && ./build/seed_tests
```

Note: `ServerRuntimeChallengeLoginTest` may flake locally on socket timing;
also observed on main without this branch's changes.

## Next safe action

- Merge after CI.
- Postgres challenge adapter Phase 1 Red, or anonymous login removal slice.

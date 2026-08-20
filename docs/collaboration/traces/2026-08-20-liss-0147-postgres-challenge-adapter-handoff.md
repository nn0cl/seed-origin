# Trace: LISS-0147 Postgres challenge/session adapter slice

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0147
- Branch: `feature/liss-0147-postgres-challenge-adapter`
- Base: main `a130a21` (PR #8 servermain merge)

## Adjudicator Approval

- Phase 1 Red / Phase 2 Green / Phase 3 Refactor approved 2026-08-20.
- Scope: Postgres-backed `PlayerChallengeClaimPort` + `PlayerSessionStorePort`,
  plus migration DDL and separable integration tests.
- Out of scope: `ServerBootstrap` challenge production wiring when env is set,
  anonymous login removal.

## Verification

```text
cmake --build build --target seed_postgres_tests seed_tests
./build/seed_tests                      # EXIT 0
./build/seed_postgres_tests             # EXIT 0 (skip when SEED_IDENTITY_DB_URL unset)
```

- Docker daemon was unavailable locally during this slice, so the Postgres-backed
  paths were verified only to the skip behavior, not against a live container.

## Next safe action

- Merge after CI.
- Then wire `SEED_CHALLENGE_AUTH` to `PostgresPlayerSessionStore` in
  `ServerBootstrap`, or start the anonymous-login removal slice.

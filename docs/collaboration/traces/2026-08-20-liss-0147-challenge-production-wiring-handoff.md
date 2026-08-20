# Trace: LISS-0147 challenge production wiring slice

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0147
- Branch: `feature/liss-0147-challenge-production-wiring`
- Base: main `b1a9348` (PR #9 Postgres adapter merge)

## Adjudicator Approval

- Phase 1 Red / Phase 2 Green / Phase 3 Refactor approved 2026-08-20.
- Scope: challenge-production bootstrap contracts, test-hook-driven production
  dispatcher selection, and authenticated session allocation path.
- Out of scope: live Postgres-backed production bootstrap, anonymous login
  removal.

## Verification

```text
cmake --build build --target seed_tests
./build/seed_tests   # EXIT 0 on one run; existing socket-related flake remains
```

- Re-runs on this machine still showed the known `ServerRuntimeChallengeLoginTest`
  instability, but the new bootstrap test path passed.

## Next safe action

- Implement live `SEED_CHALLENGE_AUTH` bootstrap using
  `PostgresPlayerSessionStore`.
- Then remove the anonymous login path.

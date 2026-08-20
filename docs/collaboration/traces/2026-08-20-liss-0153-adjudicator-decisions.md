# Trace: LISS-0153 durable policy adjudicator decisions

- Date: 2026-08-20
- Path: Architecture Path (policy closure; no Phase 1–3 code in this trace)
- Issue: LISS-0153 (parent LISS-0152)
- Planning record: AIP-0153-001 through AIP-0153-005 (implementation); this trace records policy only
- Branch: `feature/liss-0152-client-side-prediction`

## Request

Record Adjudicator-approved decisions for four blocking policy questions on
LISS-0153. Use the previously recommended options unchanged. Edit LISS-0153
docs only; do not touch seed-auth or LISS-0152 code. Do not commit.

## Decisions recorded

1. **PlayerName normalization:** early trim + exact-match, case-sensitive.
   Unicode NFC and case-insensitive uniqueness deferred to auth ledger Phase 2
   (LISS-0146–0150 follow-up).
2. **Spawn position:** early `LoginFieldSpawnSettings` with `(0,0,0)` default.
   Zone/logout spawn is second phase after auth spawn port. Collision avoidance
   undecided; same origin allowed early.
3. **Auth name ledger:** world in-memory claimed-name is provisional. seed-auth
   is authoritative for global uniqueness. Migrate to Login port that returns
   PlayerName; world validates only.
4. **Operator UI:** early `operatorSetPlayerName` + test/CLI stub. Production
   via LISS-0144 admin API. No wire player-rename command. Permission model is
   a separate ADR.

## Files changed

- `docs/issues/LISS-0153-login-field-spawn-policy.md` — status, decisions, resolved questions
- `docs/work-plans/WP-0010-client-side-prediction.md` — issue graph and next-issue note
- `docs/specs/client-side-prediction-v1.md` — out-of-scope and ambiguity alignment
- `docs/collaboration/traces/2026-08-20-liss-0153-adjudicator-decisions.md` — this trace

## References (main worktree only; link-only)

- `docs/architecture/adr/0023-player-auth-session-flow-details.md`
- `docs/specs/player-authentication-flow-v1.md`
- `docs/architecture/adr/0024-language-specific-test-layouts.md`

## Next safe action

- LISS-0153 early policy matches branch implementation; no new Phase 1 Red for
  the four decided items.
- Phase 1 Red for deferred work belongs on LISS-0146–0150 (auth ledger,
  Login PlayerName port, NFC/case-insensitive Phase 2) and future operator
  permission ADR.
- Continue LISS-0128 remainder or close LISS-0153 to `done` after Adjudicator
  review of policy docs vs branch.

## Verification

- Documentation-only change. No tests run.

## Adjudicator approval (2026-08-20)

- Adjudicator: **承認** — durable policy closure accepted unchanged.
- Issue sync: LISS-0153 `review` → `done`; phase
  `adjudicator-policy-closure-complete`.
- Committed on branch `feature/liss-0152-client-side-prediction` with WP-0010
  and spec alignment (this trace updated same date).

## Handoff (post-approval)

- **Completed:** Four blocking policy questions closed in issue + CSP spec;
  implementation on branch matches early policy.
- **Next safe action:** LISS-0128 remainder on this branch; auth ledger /
  Login PlayerName port (LISS-0146–0150); operator permission ADR when scoped.
- **Verification:** Documentation-only closure slice; no new tests required.

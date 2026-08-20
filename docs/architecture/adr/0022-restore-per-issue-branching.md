# ADR 0022: Restore Per-Issue Branching; Remove Autonomous Push-on-Completion

## Status

Accepted

## Ratification

Ratified by the Adjudicator on 2026-07-18, following a review of this
session's process failures (an ADR authored and implemented in the same
turn without a review checkpoint; repeated gaps between chat-discussed
decisions and their documentation). This ADR reverses ADR 0013's branching
decision only. ADR 0013's other decision — the Adjudicator/Developer role
boundary — is unaffected and remains in effect.

## Context

ADR 0013 established that `seed-origin` uses `main` directly for issue
work instead of the generic template baseline's per-Issue branch, and that
a completed Issue is committed and pushed to `main` with a CodeQL wait
before the next Issue starts.

While investigating this session's process failures, the Adjudicator and
agent found that `.github/copilot-instructions.md`, `.grok/rules/*.md`,
and `.cursor/rules/*.mdc` never actually carried this exception — they
consistently instructed feature-unit branches and explicitly said not to
implement issue work directly on `main` (see
`.grok/rules/03-collaboration-and-completion.md`: "do not implement issue
work directly on `main` or the trunk branch"). Only `AGENTS.md`,
`CLAUDE.md`, and `docs/collaboration/branch-commit-pr-discipline.md`
carried the seed-origin-specific override. This was already a
cross-file inconsistency that `prompt-instruction-change-control.md`'s
review rule is meant to catch, independent of today's other findings.

## Decision

1. **Per-Issue branching is restored.** `seed-origin` no longer uses the
   ADR 0013 exception. Issue work follows the generic baseline already
   described in `docs/collaboration/branch-commit-pr-discipline.md`
   (minus its own now-removed exception clause): one branch per Issue,
   Phase, or reviewable unit, following that document's naming and CI-gate
   rules.
2. **Commit remains mandatory.** An Issue is still not complete without a
   commit containing its approved changes — this is unchanged from ADR
   0013.
3. **The autonomous "push `main` + wait for CodeQL before the next Issue"
   gate is removed.** This mechanism only made sense when work landed
   directly on `main`. Push, PR creation, and CI/CodeQL review now follow
   the normal branch/PR flow in `branch-commit-pr-discipline.md` — a
   branch must pass CI before merging, but there is no special rule
   requiring the agent to push and block on CodeQL immediately after every
   commit.
4. This does **not** change the agent's own standing safety constraint
   that any `git push` requires explicit, per-instance user confirmation
   — that constraint exists independently of this repository's contract
   files and is not affected by this ADR.
5. ADR 0013's Adjudicator/Developer role-boundary decision (who holds
   phase-transition and ambiguity-decision authority) is **not** reopened
   by this ADR and remains in effect, along with ADR 0014.

## Consequences

Positive:

- Restores a mechanical review checkpoint (branch + PR + CI gate) that
  does not depend entirely on the agent correctly recognizing, in the
  moment, when to stop and ask — the exact failure mode observed this
  session with ADR 0017.
- Resolves a pre-existing inconsistency between `AGENTS.md`/`CLAUDE.md`
  and the `.github`/`.grok`/`.cursor` mirror files, which already assumed
  branch-per-issue.
- Aligns with `branch-commit-pr-discipline.md`'s existing CI-gate and
  worktree guidance, which was already written for a branch-based world.

Negative:

- Slower iteration for small, low-risk changes that previously could land
  on `main` in one step.
- Requires updating every place that referenced the old exception
  (`AGENTS.md`, `CLAUDE.md`, `branch-commit-pr-discipline.md`, ADR 0005)
  in the same reviewable unit as this ADR, per Definition of Done's status
  synchronization rule.

## Related documents

- `docs/architecture/adr/0013-project-main-branch-and-adjudicator-roles.md`
  (branching portion superseded by this ADR; role-boundary portion
  unaffected)
- `docs/architecture/adr/0005-local-issue-planning.md`
- `docs/collaboration/branch-commit-pr-discipline.md`
- `AGENTS.md`
- `CLAUDE.md`
- `docs/collaboration/traces/2026-07-18-remove-main-only-branch-exception.md`

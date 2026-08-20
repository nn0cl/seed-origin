# AI Work Trace

## Request

- Date: 2026-07-18
- User request: remove the seed-origin-specific exception that allows
  working directly on `main` without an Issue-scoped branch (ADR 0013's
  branching decision), and remove the rule requiring an autonomous push to
  `main` plus CodeQL-wait on every Issue completion (AGENTS.md's "Issue
  Completion Delivery" section) — while keeping the commit requirement.
- Current phase: documentation-only, agent operating contract change.
- Canonical issue or work plan: none (process/governance change, not
  feature work).
- AI planning record: none (process change, not sized feature work).

## Context Ledger

- Included: `AGENTS.md`, `CLAUDE.md`, `docs/collaboration/branch-commit-pr-discipline.md`,
  `docs/architecture/adr/0005-local-issue-planning.md`,
  `docs/architecture/adr/0013-project-main-branch-and-adjudicator-roles.md`,
  `.github/copilot-instructions.md`, `.grok/rules/*.md`,
  `.cursor/rules/*.mdc`.
- Omitted: unrelated contract sections (Phase Discipline content itself,
  Adjudicator/Developer role-boundary decision in ADR 0013, which is not
  reopened by this change).
- Assumptions: the Adjudicator/Developer role-boundary half of ADR 0013 is
  unaffected; only its branching/push decision is reversed. Confirmed by
  reading ADR 0013 in full before editing — it bundles two distinct
  decisions and only one is in scope here.
- Open decisions: none outstanding for this change; the request was
  explicit. This trace documents the discovery (during investigation) that
  `.github/copilot-instructions.md`, `.grok/rules/*.md`, and
  `.cursor/rules/*.mdc` already described branch-per-issue work and never
  carried the seed-origin main-only exception — meaning those mirror files
  were already inconsistent with `AGENTS.md`/`CLAUDE.md` before this
  change. This change resolves that pre-existing drift rather than
  introducing new drift.

## Routing

- Model/assistant/tool: Claude Code (interactive session), no sub-agent
  delegation for this contract-file change.
- Reason: direct, auditable single-agent edit, per
  `docs/collaboration/prompt-instruction-change-control.md`.
- Privacy constraints: none.

## AI Execution Records

### Attempt 1

- Agent: Claude Code (Sonnet 5)
- Environment: local interactive session, seed-origin repository on `main`.
- Model as displayed: claude-sonnet-5.
- Reasoning setting as displayed: not applicable (interactive CLI session).
- Estimated token range / midpoint / actual: not tracked for this session.
- Token metric/source/attribution boundary: not applicable.
- Actual token unavailable reason: session does not export per-task token
  accounting.
- Scope: (1) add ADR 0022 reversing ADR 0013's branching/push decision,
  (2) mark ADR 0013's branching portion as superseded by ADR 0022 while
  leaving its Adjudicator/Developer role-boundary decision in effect,
  (3) update ADR 0005's reference to the (now reversed) exception,
  (4) update `AGENTS.md`'s branch-policy sentence and "Issue Completion
  Delivery" section, (5) update `CLAUDE.md`'s duplicate branch-policy
  sentence, (6) update `docs/collaboration/branch-commit-pr-discipline.md`'s
  "Repository Policy" and branch-rules sections, (7) verify
  `.github/copilot-instructions.md`/`.grok/rules/*.md`/`.cursor/rules/*.mdc`
  need no further change since they already describe branch-per-issue
  work.
- Result: pending commit alongside this trace file.
- Attempt boundary: single edit pass, no retries.
- Notes: pushing to a remote branch/PR, and merging an Issue's branch into
  `main` on completion, remain subject to this agent's own standing safety
  constraint requiring explicit per-instance user confirmation before
  either action, independent of what the contract files say — that
  constraint is not created or removed by this change.

### Attempt 2 (follow-up same day)

- Scope: add the explicit rule that an Issue's branch is merged into
  `main` on completion (this was implied by the CI-gate language but not
  stated as part of what "Issue complete" means), across `AGENTS.md`,
  `CLAUDE.md`, and `branch-commit-pr-discipline.md`.
- Result: committed alongside this trace update.

## Optional Reference Total

- Value / Metric / Source / Compatibility statement: not tracked.

## Cost / Reasoning Control

- Operating path: Architecture Path (agent operating contract change,
  process/governance decision, explicit Adjudicator direction).
- Files read: `AGENTS.md`, `CLAUDE.md`,
  `docs/collaboration/branch-commit-pr-discipline.md`,
  `docs/architecture/adr/0005-local-issue-planning.md`,
  `docs/architecture/adr/0013-project-main-branch-and-adjudicator-roles.md`,
  `.github/copilot-instructions.md`,
  `.grok/rules/03-collaboration-and-completion.md`,
  `.cursor/rules/03-collaboration-and-completion.mdc`.

## Which contract file(s) changed, why, and expected behavior change

- **Files changed**: `AGENTS.md`, `CLAUDE.md`,
  `docs/collaboration/branch-commit-pr-discipline.md` (contract file per
  its inclusion in `prompt-instruction-change-control.md`'s list via
  `docs/collaboration/*.md`), plus ADR 0005 and a new ADR 0022 (ADRs are
  design-decision records referenced by the contract files, not contract
  files themselves, but are updated in the same reviewable unit per
  Definition of Done's Issue/decision status synchronization principle).
- **Why**: the Adjudicator determined, after a review of this session's
  process failures, that the main-only exception and the autonomous
  push-and-wait-for-CodeQL-before-next-issue rule reduce the review rigor
  this project needs, and that reverting to the generic per-Issue-branch
  baseline (which `.github/copilot-instructions.md`, `.grok/rules/*.md`,
  and `.cursor/rules/*.mdc` already assumed) restores it. Commit remains
  mandatory; only the "must push to `main` directly and block on CodeQL
  before the next Issue" mechanism is removed, since it was specific to
  the now-reversed main-only workflow.
- **Expected agent behavior change**: a future agent session must create a
  dedicated branch per Issue before starting Phase 1, may no longer commit
  directly to `main`, still must commit its work, and no longer treats
  "push to `main` + CodeQL wait" as a blocking gate before starting the
  next Issue — push/PR/CI review instead follows the generic branch flow
  already described in `docs/collaboration/branch-commit-pr-discipline.md`.
  The agent's own standing rule to ask before any `git push` remains
  unchanged and independent of this contract change.

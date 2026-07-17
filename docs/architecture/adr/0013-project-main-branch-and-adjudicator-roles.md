# ADR 0013: Project Main Branch and Adjudicator Role Boundary

## Status

Accepted

## Context

The upstream template's generic local-issue guidance assumes that issue work is
performed on dedicated branches. This repository has an explicit project-level
operating decision to use `main` for issue work and to commit and push completed
issues directly there. The project also needs to distinguish equal behavioral
standards for humans and AI agents from unequal decision authority.

## Decision

- `main` is the working branch for issue-scoped changes in this repository.
- A local Issue, acceptance specification, and required phase artifacts remain
  mandatory even when a separate branch is not used.
- The human Adjudicator owns phase transitions, Phase 1 test review, ADR
  acceptance, ambiguity decisions, and final responsibility.
- AI agents and humans are subject to the same safety, evidence, and review
  standards. AI agents do not acquire Adjudicator authority by producing code,
  passing tools, or receiving an autonomous execution instruction.
- A completed Issue is committed and pushed to `main`; CodeQL is inspected
  before the next Issue begins.

## Consequences

- The branch rule intentionally differs from the reusable template baseline and
  is documented rather than left as an implicit chat instruction.
- Direct main delivery increases the importance of small slices, clean working
  trees, CodeQL review, and accurate Issue status.
- Human decision points remain explicit even when implementation is delegated
  to an AI agent.

## Related documents

- `docs/collaboration/ai-human-scheme.md`
- `docs/collaboration/branch-commit-pr-discipline.md`
- `AGENTS.md`
- `CONTRIBUTING.md`

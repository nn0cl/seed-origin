# ADR 0014: Adjudicator and Developer Role Model

## Status

Accepted

## Ratification

Ratified by the Adjudicator on 2026-07-17.

## Context

The repository is developed through collaboration among humans, AI agents, and
deterministic tools. Treating "human" and "AI" as the primary role boundary is
inaccurate: a human may participate as an ordinary Developer, while an
Adjudicator is a distinct decision-making role.

## Decision

- **Adjudicator** is the decision-making role. The Adjudicator owns phase
  transitions, Phase 1 test review, ADR acceptance, ambiguity resolution, and
  final responsibility.
- **Developer** is an execution role that may be held by a human or an AI
  agent. Developers perform design intake, implementation, verification, and
  evidence recording within the approved scope.
- Human Developers and AI Developers follow the same operating rules, safety
  requirements, acceptance criteria, evidence requirements, and review gates.
- A Developer does not gain Adjudicator authority from being human, producing
  code, passing deterministic tools, or receiving an autonomous execution
  instruction.
- Deterministic tools provide repeatable verification signals but do not make
  architecture or product decisions.
- A Developer must escalate phase transitions, ADR decisions, unresolved
  ambiguity, and changes to accepted behavior to the Adjudicator.

## Consequences

- Human participation is not limited to supervision; humans may contribute
  implementation as Developers.
- AI agents are not treated as decision makers merely because they can execute
  tools or generate persuasive explanations.
- Contribution and conduct documents can define equal standards without
  erasing the authority boundary.
- Work records must identify the active role when that distinction matters.

## Related documents

- `docs/collaboration/ai-human-scheme.md`
- `CONTRIBUTING.md`
- `CODE_OF_CONDUCT.md`
- `docs/architecture/adr/0003-ai-human-collaboration-governance.md`
- `docs/architecture/adr/0013-project-main-branch-and-adjudicator-roles.md`

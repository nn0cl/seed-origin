# Agent Operating Contract

This repository is prepared for multiple AI coding agents. All agents must use
the same workflow and architectural boundaries.

## Prime Directive

No implementation without a reviewed acceptance specification.

No phase skipping.

No hidden business logic in adapters.

## Expected Workflow

1. Read `docs/architecture/agent-quickstart.md`.
2. Select the smallest matching operating path from that quickstart:
   Fast Path, Feature Path, or Architecture Path.
3. Read only the documents required by the selected path.
4. Check `docs/architecture/implementation-readiness.md` before Phase 1, 2, or
   3 starts.
5. Output the path-appropriate design note.
6. Execute only the requested phase.
7. Report Red, Green, Refactor, or Fast Path status honestly.

## Session Entry

- Treat each new session as having no prior chat context.
- Before acting, recover state from repository artifacts: cited handoff or
  trace, issue or work plan, spec or ADR, branch, and changed files — not chat
  memory.
- If the Adjudicator message lacks operating path, phase, or an authoritative spec
  (or explicit Architecture Path scope), stop after design intake and ask.
- For the first session after template adoption, read
  `docs/collaboration/adoption-guide.md` before changing target-owned files.
- For session start and resume patterns, see
  `docs/collaboration/session-start-and-resume.md`.

Relevant architecture documents:

- Quickstart: `docs/architecture/agent-quickstart.md`.
- File placement: `docs/architecture/project-structure.md`.
- Readiness checklist: `docs/architecture/implementation-readiness.md`.
- Test placement: `docs/architecture/testing-strategy.md`.
- Dependency policy: `docs/architecture/dependency-policy.md`.
- AI request routing: `docs/architecture/ai-request-routing.md`.
- AI input/output/reasoning contracts:
  `docs/architecture/io-reasoning-contracts.md`.
- AI-human collaboration scheme:
  `docs/collaboration/ai-human-scheme.md`.
- Source code quality for AI-TDD:
  `docs/collaboration/source-code-quality.md`.
- Definition of Done:
  `docs/collaboration/definition-of-done.md`.
- Model/tool routing:
  `docs/collaboration/model-tool-capability-matrix.md`.
- Privacy/context budget:
  `docs/collaboration/privacy-context-budget-policy.md`.
- Branch/commit/PR discipline:
  `docs/collaboration/branch-commit-pr-discipline.md`.
- Local issue planning:
  `docs/collaboration/local-issue-planning.md`.
- Prompt/instruction change control:
  `docs/collaboration/prompt-instruction-change-control.md`.
- Session start and resume:
  `docs/collaboration/session-start-and-resume.md`.
- C++ core and server: `include/seed/`, `src/`, and `docs/architecture/`.
- Tests and test runner: `tests/` and `CMakeLists.txt`.
- Future client: `client/` after LISS-0064 selects the client technology.

## Clean Architecture Dependency Rule

Allowed dependencies:

- Domain -> nothing project-specific.
- UseCase -> Domain and Ports.
- Adapter -> UseCase, Ports, framework SDKs, DB SDKs, file system, network.
- UI/Delivery -> application command/query contracts and presentation state.

Forbidden dependencies:

- Domain -> Adapter.
- Domain -> Framework.
- UseCase -> DB schema.
- UseCase -> migration files.
- UseCase -> UI component.
- UseCase -> framework request/command handler.
- UI -> DB.
- UI -> external provider SDK.
- Adapter -> business policy not present in UseCase or Domain.

## External Resources Must Be Ports

Represent these as ports before using concrete implementations.

- Local filesystem and binary files through file/I/O adapters.
- POSIX TCP sockets through the Connection adapter.
- GitHub Actions and CodeQL as repository security tooling.
- CMake/CTest and Xcode as build/test tooling, not domain dependencies.
- Future identity persistence through a declared storage port.
- Future client renderer and UI framework through a client adapter boundary.
- Settings storage and validation.
- Secret storage.
- Dependency policy checks.
- No external runtime service is currently required by the core.
- LLM/agent providers are untrusted development inputs, never game state.

## Adjudicator Interaction

When a decision affects architecture, capture it as an ADR. When a decision is
unknown, list it in the path-appropriate design note as an ambiguity boundary.

Every request starts from design intake. Select only the AI payload context
needed for the task, define lightweight VO or DTO candidates when clear, and
route subtasks to an appropriate model, code assistant, or deterministic tool.
When AI or model output is involved, define input, output, and reasoning
evidence contracts before implementation.

Use the full `[THOUGHT]` scaffold only for Feature Path and Architecture Path
work. For Fast Path work, use a compact design note that states scope, omitted
context, deterministic checks, and why the full scaffold is unnecessary.

When handing off or stopping before completion, use
`docs/templates/agent-handoff.md`. When asking the Adjudicator for approval, use the
review points from `docs/templates/adjudicator-review.md`.

Generated source code must minimize human cognitive load. Prefer clear
responsibility boundaries, small functions, straightforward names, and
reviewable tests. Do not compress implementation into dense code just to be
minimal.

Before reporting completion, check `docs/collaboration/definition-of-done.md`.
Create AI work traces under `docs/collaboration/traces/` when the trace policy
requires it. This repository uses a dedicated branch per Issue, Phase, or
reviewable unit, per `docs/collaboration/branch-commit-pr-discipline.md`
(ADR 0022; this reverses the earlier ADR 0013 main-only exception). Do not
implement issue work directly on `main`.
For feature work, identify local issue or GitHub issue dependencies before
creating the branch.

## Issue Completion Delivery

- An issue is not complete until its approved changes are committed on its
  branch.
- An issue's branch is merged into `main` on completion, following the
  normal branch flow in `docs/collaboration/branch-commit-pr-discipline.md`
  — a branch must pass CI before merging into `main`. There is no separate
  rule requiring an immediate push-and-wait-for-CodeQL step outside that
  normal flow.
- Any `git push` and any merge into `main` still require explicit,
  per-instance user confirmation before they happen, independent of this
  section.
- The completion report must include the commit and current branch/PR
  state. If commit is blocked, keep the issue open and record the blocker
  instead of reporting completion.

# Architecture Overview

The project uses Clean Architecture with `<FILL IN: local-first / cloud /
hybrid>` runtime assumptions.

The selected stack is `<FILL IN: e.g. Tauri + Rust + React, or Node + Next.js,
or Django + HTMX>`. State which part of the stack owns the application core
and which owns UI presentation.

## Layers

### Domain

Pure `<FILL IN: your core domain concepts>` behavior.

Must not depend on:

- any UI framework.
- SQL schemas, ORM structs, vector DB SDKs, or file-system APIs.
- LLM SDKs, cloud AI SDKs, or third-party provider APIs.

### UseCase

Coordinates domain behavior through ports.

Examples (replace with your project's actual use cases):

- `<Example use case A>`.
- `<Example use case B>`.

### Ports

Interfaces owned by the application core.

Ports isolate every external resource named in `CLAUDE.md` / `AGENTS.md`
under "External Resources Must Be Ports".

### Adapters

Framework and infrastructure implementations.

Adapters may use framework APIs, infrastructure libraries, DB or vector DB
SDKs, external file layouts, API clients, and provider SDKs.

Adapters must not define business policy.

### Front-End / Delivery

The delivery layer (UI, CLI, HTTP API) presents domain state and collects
user input.

It must not own:

- confidence, trust, or merge policy for AI-derived data.
- validation or secret-storage policy.
- any policy that belongs in a use case.

## Runtime Direction

`<FILL IN: where does this run — local device, server, browser, hybrid — and
which parts are optional/replaceable providers (e.g. cloud AI, external
APIs)?>`

## Selected Technology

- `<Runtime/shell>`.
- `<Application language>`.
- `<UI framework>`.
- `<Package manager>`.
- `<Distribution goal, if relevant>`.

## Detailed Rules

- `project-structure.md`: where files belong.
- `testing-strategy.md`: AT-TDD test placement.
- `implementation-readiness.md`: checklist before coding.
- `dependency-policy.md`: package dependency checking policy.
- `ai-request-routing.md`: AI payload selection and task routing.
- `io-reasoning-contracts.md`: AI input/output/reasoning contracts.
- `external-resource-adoption-contract.md`: optional contract for adopting
  AI-generated or human-sourced external content/data resources.
- `<Add one line per stack-specific architecture document you create.>`

## Accepted Decisions

- `adr/0001-design-first-ai-request-routing.md`
- `adr/0002-input-output-reasoning-contracts.md`
- `adr/0003-ai-human-collaboration-governance.md`
- `adr/0004-human-readable-source-code-quality.md`
- `adr/0005-local-issue-planning.md`
- `adr/0006-prompt-instruction-change-control.md`
- `adr/0007-trunk-oriented-branching.md`
- `adr/0008-template-update-propagation.md`
- `adr/0009-bug-planning-and-ai-usage-records.md`
- `adr/0010-ai-failure-recovery-and-runner-cli-contract.md`
- `adr/0011-external-resource-adoption-contract.md`

## Remaining Technology Evaluation

List technology choices still open for ADR decision, e.g.:

- `<Persistence choice>`.
- `<Vector DB / search choice>`.
- `<Embedding model choice>`.
- `<External provider choice>`.

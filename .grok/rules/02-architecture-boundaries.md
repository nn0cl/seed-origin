# Grok Agent Instructions: Architecture Boundaries

## Phase Gate

Only execute the phase explicitly requested by the human Adjudicator.

Do not implement ahead of the current phase. Do not "helpfully" generate
production code during Phase 1.

### Phase 1: Red - Failing Tests Only

Generate tests only.

- Do not write production implementation.
- Depend on ports or interfaces for all external resources.
- Mock every external resource listed below under "External Resources Must
  Be Ports".
- Assertions must match the Gherkin `Then` clauses exactly.
- Red is acceptable as compile failure when interfaces or use cases do not
  yet exist, or as test failure when skeletons exist.

### Phase 2: Green - Minimal Implementation

Generate only the minimum production implementation required to pass
reviewed Phase 1 tests.

- Do not modify tests to make them pass.
- Keep business logic in Domain or UseCase layers.
- Keep UI components, framework request/command handlers, database structs,
  provider clients, and file adapters free of business decisions.
- Do not add behavior not specified by EARS, Gherkin, or reviewed tests.

### Phase 3: Refactor and Reviewer Empathy

Refactor only after Green. Behavior and assertions must not change.

After refactoring, output:

```markdown
### 変更の要約 (PR Summary)
- **何を目的として何を変更したか**: ...

### 残存リスク・検証の溝 (Verification Gap)
- **AIが推測で補った部分、またはハルシネーションが発生しやすい箇所**: ...
- **人間がコードレビューで重点的に見るべきポイント**: ...
```

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

Represent these as ports before using concrete implementations. The current
external boundaries are:

- local filesystem and binary-file adapters;
- POSIX TCP socket adapter;
- future identity persistence adapter;
- future client renderer/UI adapter;
- GitHub Actions and CodeQL tooling;
- CMake/CTest and Xcode build tooling.
- Settings storage and validation.
- Secret storage.
- Dependency policy checks.
- No external runtime service is currently required by the core.
- LLM/agent output is untrusted development input.

## Anti-Hallucination Rules

- Do not invent APIs, model names, vector dimensions, database schemas,
  migrations, or external folder/service conventions.
- Do not include unrelated files, full transcripts/documents, full data
  exports, or secrets in AI request payloads.
- Do not treat free-form AI prose as trusted domain data. Validate output
  schemas, source references, confidence, and review status before use.
- Do not generate dense or multi-responsibility code. Keep source code
  appropriately split and readable for human review.
- If a dependency is unknown, add an interface boundary or an ADR question.
- If a behavior is not in the specification, do not implement it.

# LISS-0140: C++20移行前のコンパイル復旧

## Metadata

- Local issue ID: LISS-0140
- GitHub issue: 未作成
- Status: review
- Phase: phase-2-implementation
- Type: bugfix + build foundation
- Priority: critical
- Initial planning size: M
- Current planning size: M
- Reclassification reason: N/A
- Owner/agent: Codex
- Related branch: `main`

## Summary

C++20移行前に、現在の`main`で発生しているコンパイル不能状態を復旧する。
このIssueは仕様追加ではなく、既存コミットで壊れたヘッダ依存、構文、
namespace、初期化の不整合を直すための安全ゲートである。

## Acceptance Notes

- `ServerRuntime.h` が `network::WorldUpdate` を使うため必要なヘッダを
  自己完結的にincludeする。
- Protocol version定数を `NetworkCommand.h` と `WorldUpdate.h` の双方から
 安全に参照できる。
- `WorldInputQueue` のrequest ID重複判定が構文的に正しい。
- `Player(int64_t, Status, Position)` が `Position` をmember initializerで
  初期化し、未初期化/不正default construct要求を発生させない。
- テストtranslation unitのnamespace/型名参照が現在のヘッダ定義に一致する。
- テストや成果物を実行せず、CMake buildまたは構文検査でコンパイル可能性を
  確認する。

## Dependencies

- Parent: WP-0008
- Depends on: なし
- Blocks: LISS-0141, LISS-0142
- Related: ADR-0015, `CMakeLists.txt`, `include/seed/`, `src/`, `tests/`

## Adjudicator Decision Points

- テスト・成果物の実行は禁止。ビルド/静的確認のみ許可。
- このIssueでは実行時挙動の合否は確定しない。

## Context

- Included: compile errors from CMake build, directly touched headers/sources,
  directly affected test translation units.
- Omitted: runtime socket behavior, test execution, CLI execution, server
  execution, feature behavior changes.
- Assumptions: existing tests are compiled as translation units only; assertions
  are not evaluated in this issue.

## AI Planning Records

### AIP-0140-001

- Status: accepted
- Created by:
  - Agent/environment: Codex / local workspace
  - Model as displayed: fugu
  - Reasoning setting as displayed: N/A
  - N/A reason: runtime does not expose a separate user-facing reasoning label
- Created at: 2026-07-17
- Planning size: M
- Intended execution route: Feature Path Phase 2 bugfix under accepted scope
- Intended scope: restore compilation by fixing header dependencies, syntax,
  namespace references, and initializer-list safety
- Estimated token range: 4k-8k
- Estimated token midpoint: 6k
- Token metric: planning estimate
- Estimation basis: several compile failures across core and tests
- Assumptions: no tests/artifacts executed
- Confidence: medium
- Revises: N/A
- Revision reason: N/A
- Superseded by: N/A

## References

- ADR-0015: C++20 language standard adoption
- WP-0008: C++20 migration and modernization

## Work Notes

- 2026-07-17: Initial CMake build failed before test execution because core
  did not compile.
- 2026-07-17: Added shared `Protocol.h`, fixed direct header include, syntax,
  namespace, and initialization issues.

## Verification

- Pending: CMake build and/or syntax-only checks only.
- Not allowed: `ctest`, `seed_tests`, `seed_cli`, server runtime.

## Completion Evidence

- CMake C++20 configure/build completed successfully, including `seed_core`,
  `seed_cli`, and the test translation units. The generated executables were
  not run.
- Header self-containment, protocol constant visibility, request-ID syntax,
  `Player` initialization, and affected test namespace references compile.

# WP-0008: C++20移行と安全性リファクタ

## Goal

C++11ベースのビルド・ソース記法をC++20へ移行し、同時にビルド不能状態、
ヘッダ依存漏れ、初期化不備、メモリ破壊につながる所有権・構文・名前空間の
危険を一通り修正する。

## Scope

- In:
  - CMakeのC++20設定。
  - C++標準移行を決めるADR。
  - 移行作業を追跡するLISS Issue。
  - `include/seed/`、`src/`、`tests/` のコンパイル可能性修正。
  - 危険バグ修正: ヘッダ自己完結性、プロトコル定数の単一化、構文ミス、
    namespace不一致、非default constructibleメンバーのbody代入constructor。
  - 保守的なC++20 idiomへの更新（`<cstdint>`/`inline constexpr`/
    `[[nodiscard]]`/range-for/initializer list）。
- Out:
  - テスト実行、`seed_tests`実行、`seed_cli`実行、サーバ起動。
  - 新しいゲーム仕様、プロトコル値変更、永続化、client renderer選定。
  - C++20 modules/coroutines、`std::span`/`std::optional`公開API再設計、
    strong typed-ID全面導入。
  - Xcode projectの広範な手編集（必要なら後続Issueで同期）。

## Issue Graph

| Issue | Status | Initial size | Current size | Planning record | Depends on | Blocks | Branch |
| --- | --- | --- | --- | --- | --- | --- | --- |
| LISS-0140 | review | M | M | AIP-0140-001 | - | LISS-0141, LISS-0142 | main |
| LISS-0141 | review | M | M | AIP-0141-001 | LISS-0140, ADR-0015 | LISS-0142 | main |
| LISS-0142 | review | L | L | AIP-0142-001 | LISS-0141 | future C++ API cleanup | main |

## AI Planning Records

### AIP-WP-0008-001

- Status: accepted
- Created by:
  - Agent/environment: Codex / local workspace
  - Model as displayed: fugu
  - Reasoning setting as displayed: N/A
  - N/A reason: runtime does not expose a separate user-facing reasoning label
- Created at: 2026-07-17
- Planning size: L
- Intended execution route: Architecture Path for ADR/process decision, then
  Feature/Refactor execution under the accepted migration scope
- Intended scope: C++20 standard bump, compile restoration, header dependency
  cleanup, ownership/initialization safety, conservative modernization
- Estimated token range: 12k-24k
- Estimated token midpoint: 18k
- Token metric: planning estimate
- Estimation basis: cross-module C++ headers/sources/tests, existing main
  compile breaks, and documentation synchronization
- Assumptions: no test/artifact execution; no new dependency; no behavior-level
  protocol change
- Confidence: medium
- Revises: N/A
- Revision reason: N/A
- Superseded by: N/A

## Recommended Order

1. LISS-0140: restore compile without changing runtime behavior.
2. LISS-0141: adopt C++20 in CMake and architecture documentation.
3. LISS-0142: apply safe modern idioms and memory-safety refactors.

## Current Review Gate

- Issues: LISS-0140, LISS-0141, LISS-0142
- Reason: the implementation and compile/static verification are complete;
  human review remains before these migration Issues are marked done.
- Runtime tests and executable/server execution remain intentionally pending by
  the Adjudicator's safety constraint.

## Risks

- Tests cannot be executed by instruction, so runtime behavior must be reported
  as unverified.
- POSIX socket tests previously require sandbox/network permission; this plan
  avoids execution entirely.
- Large mechanical diffs can hide behavior changes; keep edits conservative and
  compile-driven.
- Xcode project settings may remain stale until a dedicated sync issue.

## Verification Plan

- Allowed:
  - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
  - `cmake --build build`
  - `clang++ -std=c++20 -fsyntax-only ...`
  - `git diff --check`
  - static source searches for forbidden/legacy idioms in touched scope
- Forbidden by Adjudicator instruction:
  - `ctest`
  - direct execution of `build/seed_tests`
  - direct execution of `build/seed_cli`
  - server/listener runtime execution

## Work Notes

- 2026-07-17: Baseline compile revealed multiple pre-existing breaks on
  `main`: missing `WorldUpdate.h` include, protocol constant visibility leaks,
  missing parenthesis in `WorldInputQueue`, `Player` constructor failing to
  initialize `Position`, stale test namespace references.
- 2026-07-17: User clarified no tests/artifacts may be executed; verification
  strategy changed to build/static-only.
- 2026-07-17: C++20 configure/build completed successfully. Warning-enabled
  build completed after fixing mismatched forward declarations, aggregate
  initializers, signed-size comparison, and shadowed parameters. No test or
  executable was run.

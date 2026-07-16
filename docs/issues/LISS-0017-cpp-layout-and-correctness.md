# LISS-0017: C++プロジェクト構成と既知不具合の整理

## Metadata

- Local issue ID: LISS-0017
- GitHub issue: 未作成
- Status: completed
- Phase: phase-3-verification
- Type: refactor + bugfix
- Priority: high
- Initial planning size: L
- Current planning size: L
- Reclassification reason: N/A
- Owner/agent: Codex
- Related branch: feature/LISS-0017-cpp-layout-correctness

## Summary

一般的なC++プロジェクト構成へソースを整理し、調査済みの明らかな不具合を修正する。既存の `seed` 系データ処理と `seeds` 系ゲーム状態処理の機能を保持し、Xcodeプロジェクトの参照を新しい配置へ合わせる。

## Acceptance Notes

- ソースを `src/`、ヘッダを `include/`、テストを `tests/` に整理する。
- `seed.xcodeproj` が新しいファイル配置を参照する。
- `Data` の取得処理が引数へ値を返す。
- `Status` のHP/MP設定が正しいメンバーを更新する。
- `Position::getPlayerId` と `Field::setPlayer` の再帰バグを解消する。
- `Buff`、`Player`、`Action` の所有権を二重解放しない形にする。
- `main.cpp` の到達不能な処理とユーザー固有の絶対パスを整理する。
- 可能な範囲でC++コンパイルまたは静的な参照検証を実行する。

## Dependencies

- Parent: なし
- Depends on: なし
- Blocks: なし
- Related: `docs/architecture/seed-implementation.md`

## Adjudicator Decision Points

- 古いXcodeプロジェクトを維持するか、ソース配置に合わせて最低限更新する。
- 既存の古いテストAPIを互換維持するか、現在の安全なAPIへ移行する。

## Context

- Included: 直下のC++ソース、ヘッダ、`seed.xcodeproj`、既知の設計上の不具合。
- Omitted: 外部ライブラリ導入、ネットワーク仕様の再設計、ゲームルールの新規設計、完全なUML復元。
- Assumptions: 既存の公開APIを可能な限り維持し、機能変更より安全性とビルド可能性を優先する。

## AI Planning Records

### AIP-0017-001

- Status: proposed
- Created by:
  - Agent/environment: Codex desktop / local workspace
  - Model as displayed: GPT-5
  - Reasoning setting as displayed: N/A (not exposed)
  - N/A reason: UI does not expose a separate reasoning label
- Created at: 2026-07-17
- Planning size: L
- Intended execution route: Feature Path, design intake followed by implementation and deterministic verification
- Intended scope: source layout, Xcode references, memory ownership, data/state correctness, executable entry point
- Estimated token range: 8k-16k
- Estimated token midpoint: 12k
- Token metric: planning estimate
- Estimation basis: multiple modules, legacy C++ APIs, Xcode project references, and no current automated test runner
- Assumptions: no new third-party dependency and no external service is required
- Confidence: medium
- Revises: N/A
- Revision reason: N/A
- Superseded by: N/A

## Work Notes

- 2026-07-17: Source inspection found two merged implementation areas: binary/file data handling and player/status/field handling.
- 2026-07-17: Design intake completed; implementation began after this issue and work plan were recorded.
- 2026-07-17: Moved sources, headers, and tests to `src/`, `include/seed/`, and `tests/`; synchronized the Xcode groups.
- 2026-07-17: Fixed Data/Status/Position/Field correctness issues, copy ownership hazards, legacy file I/O, listener hang, and the hard-coded CLI entry point.

## Verification

- `plutil -lint seed.xcodeproj/project.pbxproj` passed.
- `clang++ -std=c++17 -Wall -Wextra -Iinclude/seed -fsyntax-only src/*.cpp` passed with non-fatal legacy warnings.
- The four test translation units, including the retained legacy test stub, passed the same syntax check.
- A temporary CLI build passed and prints usage when invoked without input/output arguments.
- 2026-07-17: Acceptance conditions satisfied; implementation is ready to commit on the feature branch.

# LISS-0018: scode world 実装の仕様整理と C++ 統合

## Metadata

- Local issue ID: LISS-0018
- GitHub issue: 未作成
- Status: ready
- Phase: phase-0-design
- Type: feature + refactor
- Priority: high
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: 未作成

## Summary

`/Users/nn0cl/Documents/git/scode` の world 関連実装を仕様化し、`seed-origin` のC++モデルへ段階的に取り込む準備を行う。Javaコードの直接移植ではなく、既存の `Player`、`Position`、`Status`、`Buff`、`Action`、`Field` との責務境界を定めてから実装する。

## Acceptance Notes

- world のライフサイクル、グリッド環境、アクション、効果、状態集計の仕様が文書化されている。
- scode と seed-origin のモデル対応表がある。
- scode 側の既知不具合が移植時の注意点として記録されている。
- 取り込み単位と、DB/通信を初期スコープから外す判断が記録されている。
- 実装開始前に、`world-model` から `world-runtime` までの依存順が確認できる。

## Scope

Included: world コアの仕様整理、C++モデルへの対応づけ、段階的統合計画、受入テスト方針。

Excluded: Javaコードの直接コピー、MyBatis/MySQL移植、JAX-RS通信移植、ゲームルールの追加設計。

## Work Notes

- 2026-07-17: `scode` の `World.java`、`WorldStatus.java`、`Grid.java`、モデル群、`WorldTest.java`、履歴を調査。
- 2026-07-17: `docs/architecture/world-integration-spec.md` に仕様と統合方針を記録。

## Dependencies

- Parent: LISS-0017
- Depends on: 既存C++の所有権修正と `Field` のキュー契約
- Blocks: world コア実装

## AI Planning Records

### AIP-0018-001

- Status: proposed
- Created by: Codex desktop / local workspace
- Model as displayed: GPT-5
- Created at: 2026-07-17
- Planning size: L
- Intended execution route: Feature Path, design intake followed by model, grid, effects, actions, runtime implementation
- Assumptions: 初期統合はインメモリC++実装とし、DB・通信は後続Issueへ分離
- Confidence: medium

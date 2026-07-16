# LISS-0018: scode world 実装の仕様整理と C++ 統合

## Metadata

- Local issue ID: LISS-0018
- GitHub issue: 未作成
- Status: in_progress
- Phase: phase-0-design
- Type: feature + refactor
- Priority: high
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: 未作成（子Issueごとにfeature-unit branchを作成）

## Summary

`/Users/nn0cl/Documents/git/scode` の world 関連実装を仕様化し、プレイヤーが3次元空間へログインして魔法や攻撃で対象物と環境エーテルへ影響を与える、権威型ゲームサーバー兼局所環境シミュレーションとして `seed-origin` のC++モデルへ段階的に取り込む準備を行う。Javaコードの直接移植ではなく、既存の `Player`、`Position`、`Status`、`Buff`、`Action`、`Field` との責務境界を定めてから実装する。

## Acceptance Notes

- world のライフサイクル、グリッド環境、アクション、効果、状態集計の仕様が文書化されている。
- scode と seed-origin のモデル対応表がある。
- scode 側の既知不具合が移植時の注意点として記録されている。
- 取り込み単位と、DB/通信を初期スコープから外す判断が記録されている。
- 実装開始前に、`world-model` から `world-runtime` までの依存順が確認できる。
- グリッド更新の計算量、境界外セル生成、更新周期に関する性能制約が定義されている。
- プレイヤー・対象物・魔法・環境エーテル・減衰・属性伝導率の関係が定義されている。
- 魔法の結果が発動時点の環境エーテルと過去イベントに依存し、同一ワールド状態からは同じ結果になることが定義されている。
- 移行を安全性ゲート、モデル、エーテル場、イベント、効果、ランタイムの子Issueへ分解し、依存順が明示されている。
- バグ発見時の記録、Issue化、Fast Path適用条件、修正タイミングが定義されている。
- TDDテストを各実装IssueのPhase 1成果物として扱い、今回の計画段階ではテスト実行を行わないことが明示されている。

## Scope

Included: world コアの仕様整理、C++モデルへの対応づけ、段階的統合計画、受入テスト方針。

Excluded: Javaコードの直接コピー、MyBatis/MySQL移植、JAX-RS通信移植、ゲームルールの追加設計。

## Work Notes

- 2026-07-17: `scode` の `World.java`、`WorldStatus.java`、`Grid.java`、モデル群、`WorldTest.java`、履歴を調査。
- 2026-07-17: `docs/architecture/world-integration-spec.md` に仕様と統合方針を記録。
- 2026-07-17: 移行をLISS-0019〜LISS-0024へ分解。安全性ゲートが解除されるまで実装・コンパイル・テスト実行を行わない計画へ変更。

## Dependencies

- Parent: LISS-0017
- Depends on: 既存C++の所有権修正と `Field` のキュー契約
- Blocks: world コア実装

## Child Issues

- LISS-0019: 移行安全性ゲートとC++所有権契約
- LISS-0020: Worldのエンティティ・座標・対象物モデル
- LISS-0021: EtherFieldのデータ構造・減衰・伝導率
- LISS-0022: 決定論的イベント・効果解決
- LISS-0023: World runtimeと時間進行
- LISS-0024: 通信境界・スナップショット・運用受入条件

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

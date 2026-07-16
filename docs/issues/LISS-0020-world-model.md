# LISS-0020: Worldエンティティ・座標・対象物モデル

## Metadata

- Local issue ID: LISS-0020
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: feature
- Priority: high
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: `main`

## Summary

プレイヤー、NPC、対象物、座標、属性、HP/MP、コマンド制約をworld共通モデルとして定義する。既存の `Field` や `Player` を直接World化せず、対象物へ効果を適用できる抽象を設ける。

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0019
- Blocks: LISS-0021, LISS-0022

## Acceptance Notes

- Entity ID、位置、状態、属性の所有権が定義されている。
- プレイヤーと対象物を同じ効果適用経路で扱える。
- 3次元距離と浮動小数点誤差の扱いが決まっている。
- 魔法の基礎威力、属性、射程、リソース制限、対象種別を表現できる。
- 状態コピーと読み取り専用参照のテストをPhase 1成果物として作成する。
- テストは今回実行しない。

## Adjudicator Decision Points

- 対象物をPlayerの派生型にするか、共通Entity値として持つか。
- 座標の量子化単位と境界をどう定めるか。
- 浮動小数点距離を固定小数点へ移行する必要があるか。

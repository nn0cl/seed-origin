# LISS-0021: EtherFieldのデータ構造・減衰・伝導率

## Metadata

- Local issue ID: LISS-0021
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: feature + performance
- Priority: high
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: `feature/LISS-0021-ether-field`

## Summary

3次元の属性エーテル場を、全セル常時更新に依存せず管理する。属性ごとの減衰、相互作用、基準値からの変化による伝導率、魔法からの注入、近傍伝播を定義する。

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0019, LISS-0020
- Blocks: LISS-0022, LISS-0023

## Acceptance Notes

- 固定配列、チャンク、アクティブセル集合の比較と選定理由が記録されている。
- 境界外セルを生成しない。
- エーテル値が属性ごとに減衰する。
- `abs(current - baseline)` に基づく伝導率が上限・下限内で計算される。
- 魔法エーテルの注入と周辺伝播が、対象物への直接効果と分離される。
- 同一入力・同一場状態から同一結果になる。
- 性能評価用テスト・ベンチマークの仕様を作るが、今回は実行しない。

## Adjudicator Decision Points

- 全セル更新、アクティブセル更新、遅延減衰の採用方式。
- 減衰、注入、伝播、伝導率計算の順序。
- 属性相互作用が二次イベントを生成する条件。

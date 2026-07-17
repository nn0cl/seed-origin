# LISS-0113: Combat／Spell IntentのWorld適用

- Status: review
- Phase: phase-1-red
- Type: feature + security
- Priority: high
- Parent: LISS-0046
- Depends on: LISS-0112, LISS-0020, LISS-0022

## 目的

Combat／Spell IntentをField上のPlayerへ決定論的に適用する。攻撃者・対象の存在、3D距離、
威力上限を全件検証してから、共通input sequence順に対象Statusへダメージを反映する。

## 受入条件

- 攻撃者と対象がFieldに存在する。
- 3D距離が100以内である。
- 威力が有限・正・100000以下である。
- 事前検証失敗時に同一frame内の対象HPを変更しない。
- 適用はinput sequence順で行う。
- Spell elementを保持したまま適用境界を通過する。
- 環境エーテル、属性伝導率、耐性、MP消費、NPC対象は後続Issueで扱う。

## 実装資料

FieldへCombat／Spellの対象・距離・威力検証とStatusダメージ適用を追加し、
WorldFrameApplierから統合入力として接続した。テスト・ビルドは実行していない。

## 次のIssue

環境エーテル状態、属性値、減衰、伝導率をWorldモデルへ追加し、Spell適用結果へ影響させる。

# LISS-0108: MovementIntentのWorld frame適用

- Status: review
- Phase: phase-1-red
- Type: feature + safety
- Priority: high
- Parent: LISS-0048
- Depends on: LISS-0106, LISS-0107

## 目的

MovementIntentQueueの入力をWorldFrameApplierへ接続し、対象Playerを全件検証した後に
Fieldのposition queueへ到着順で投入する。対象不在やEvent生成失敗時はIntentを復元する。

## 受入条件

- 全MovementIntentの対象Playerを適用前に検証する。
- 検証成功後、sequence順にFieldへ移動を投入する。
- FieldのprocessFrameで移動を1フレームとして確定する。
- 対象Player不在時にIntentを失わず、元の順序でqueueへ復元する。
- WorldUpdate生成失敗時にもIntentを復元する。
- 移動と攻撃・魔法の同一フレーム内優先順位は別Issueで確定する。

## 実装資料

WorldFrameApplierへMovementIntentQueue接続を追加し、対象検証・Field適用・失敗時復元を
実装した。テスト・ビルドは実行していない。

## 次のIssue

LISS-0109で移動IntentとActionQueueを単一のframe入力列へ統合し、異なる入力種別の到着順と
適用順を決定論的に固定する（実装済み）。

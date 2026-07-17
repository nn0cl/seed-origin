# LISS-0110: Runtime／WorldFrameApplierの統合入力移行

- Status: review
- Phase: phase-1-red
- Type: architecture + deterministic simulation
- Priority: critical
- Parent: LISS-0056
- Depends on: LISS-0109, LISS-0103, LISS-0106

## 目的

ServerRuntimeのAction専用frame結果をWorldInputQueue／WorldInputTickへ移行し、Actionと
Movementを同一sequence列でWorldFrameApplierへ渡す。

## 受入条件

- RuntimeのsubmitAction／submitMovementが統合WorldInputQueueへ入る。
- processFrameがWorldFrameInputsを返し、worldTickと入力順を保持する。
- Runtime停止時に統合入力を破棄する。
- WorldFrameApplierがAction／Movementを入力sequence順に受け取れる。
- 移動対象の事前検証とAction検証を行う。
- 旧ActionQueue／MovementIntentQueue APIを直ちに削除せず、既存利用者を壊さない。

## 実装資料

WorldInputTickを追加し、ServerRuntimeのAction投入・frame切り出しを統合入力へ移行した。
WorldFrameApplierへWorldFrameInputsの適用入口を追加した。テスト・ビルドは実行していない。

## 次のIssue

LISS-0111でWorldFrameInputsからMovement／Actionの適用結果を同一sequenceでEvent化し、
Fieldの既存position queue／action queueが持つ種別別処理順を解消する（実装済み）。

# LISS-0107: 移動Commandのframe入力化

- Status: review
- Phase: phase-1-red
- Type: feature + deterministic simulation
- Priority: high
- Parent: LISS-0048
- Depends on: LISS-0078, LISS-0104

## 目的

移動Commandを検証した後、Fieldへ即時反映せず、session ID付きのMovementIntentとして
蓄積できる通信境界を追加する。サーバーの論理frameでまとめて適用するための入力を作る。

## 受入条件

- 有限値、範囲、正のsession IDを検証してからqueueへ入れる。
- intentに到着sequenceを付与し、takeFrameでFIFOを保持する。
- queue経路ではCommand受信時にFieldを変更しない。
- 容量超過・不正入力時にqueueを部分変更しない。
- 既存のField直接経路は互換のため残すが、新しいRuntime統合では使用しない。
- MovementIntentをFieldへ適用する処理は別Issueで行う。

## 実装資料

`MovementIntentQueue`とqueue対応のMovementCommandHandlerコンストラクタを追加した。
テスト・ビルドは実行していない。

## 次のIssue

MovementIntentをActionQueue／WorldFrameApplierへ接続し、同一server frame内の移動順序と
Field適用失敗時の扱いを確定する。

# LISS-0111: 統合入力のEvent化とField適用順

- Status: review
- Phase: phase-1-red
- Type: architecture + deterministic simulation
- Priority: critical
- Parent: LISS-0023
- Depends on: LISS-0110, LISS-0080, LISS-0106

## 目的

WorldFrameInputsのAction／Movementを共通sequence順のWorldUpdate Eventへ変換し、Fieldも
同じ入力列を順番どおり処理する。Fieldのposition queueを先に全適用する既存経路による
種別間の順序逆転を避ける。

## 受入条件

- MovementとActionの両方をWorldUpdate Eventとして表現する。
- Eventのpayloadに入力種別と共通input sequenceを含める。
- Eventのsequenceが入力列順に増加する。
- Fieldが入力列を事前検証してから同じ順序で移動／Actionを適用する。
- 対象不在・不正Action時にFieldを部分変更しない。
- 既存のField::processFrameは互換のため残すが、統合入力経路では使用しない。

## 実装資料

WorldFrameUpdateBuilderのWorldFrameInputs対応、Fieldの統合入力処理、WorldFrameApplierの
統合入力適用を追加した。テスト・ビルドは実行していない。

## 次のIssue

攻撃・魔法Commandを安全なCombat／Spell Intentへ変換し、対象・距離・威力・環境エーテル
計算をWorld frameへ接続する。

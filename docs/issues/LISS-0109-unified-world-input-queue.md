# LISS-0109: Action／Movement統合WorldInputQueue

- Status: review
- Phase: phase-1-red
- Type: architecture + deterministic simulation
- Priority: critical
- Parent: LISS-0023
- Depends on: LISS-0079, LISS-0107, LISS-0108

## 目的

ActionQueueとMovementIntentQueueのsequence空間を統合し、異なる入力種別を跨いだ到着順を
1つのFIFOで保持する。これにより、同一server frame内のAction／移動の適用順を推測せずに
決定できるようにする。

## 受入条件

- ActionとMovementを同一WorldInputQueueへ投入できる。
- 種別に関係なく単調増加する共通sequenceを付与する。
- takeFrameが投入順序を保持する。
- 不正Action、不正session ID、非有限移動値を投入しない。
- queue容量超過時に既存入力を変更しない。
- 既存ActionQueue／MovementIntentQueueのRuntime置換とWorld適用は後続Issueで扱う。

## 実装資料

`WorldInputQueue`とAction／Movementの統合入力型を追加し、種別を跨いだFIFOと検証の
受入テスト資料を追加した。テスト・ビルドは実行していない。

## 次のIssue

ServerRuntimeとWorldFrameApplierをWorldInputQueueへ移行し、共通sequence順に移動とActionを
適用する。

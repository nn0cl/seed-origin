# LISS-0105: FrameActionsからWorldUpdateへの変換境界

- Status: review
- Phase: phase-1-red
- Type: architecture + deterministic simulation
- Priority: critical
- Parent: LISS-0049
- Depends on: LISS-0080, LISS-0087, LISS-0088, LISS-0104

## 目的

server frameで確定したFrameActionsを、クライアント同期へ渡せるWorldUpdate Eventへ
変換する。Actionの到着sequence、World tick、update sequence、eventIdを保持し、後続の
World適用器とSnapshot配信を分離する。

## 受入条件

- Actionの順序を保ったEvent列を生成する。
- EventのworldTickが入力FrameActionsのtickと一致する。
- update sequenceとeventIdが正数かつ単調増加する。
- 空フレームはEventを生成しない。
- 生成したEventが既存のWorldUpdate検証を通過する。
- ActionのPlayer／Field状態をこの変換層が直接変更しない。

## 実装資料

`WorldFrameUpdateBuilder`と受入テスト資料を追加し、FrameActionsを検証済みEvent列へ
変換する境界を実装した。payloadは現時点でAction種別とAction sequenceの最小情報に
限定している。テスト・ビルドは実行していない。

## 次のIssue

LISS-0106でEventを決定論的に適用し、Fieldへ反映する（実装済み）。

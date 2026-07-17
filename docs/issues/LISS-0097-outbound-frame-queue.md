# LISS-0097: 接続単位のbounded outbound frame queue

- Status: review
- Phase: phase-1-red
- Type: feature + reliability
- Priority: critical
- Parent: LISS-0095
- Depends on: LISS-0096, LISS-0091

## 目的

LoginResponseや将来のWorldUpdateを接続単位で保持し、送信処理とWorld／Command処理を
分離する。キューはFIFOであり、上限を超えた入力を部分的に受け入れない。

## 受入条件

- フレームを到着順に取り出せる。
- 空フレーム、最小ヘッダー未満、上限超過を拒否する。
- キュー上限到達後のenqueueは失敗し、既存要素を変更しない。
- popは空キューで安全に失敗し、clearで切断時の残留フレームを破棄できる。
- ソケットI/O、部分書き込み、再送、切断判定は実装しない。

## 実装資料

`OutboundFrameQueue`とFIFO・入力検証・容量超過の受入テスト資料を追加した。
テスト・ビルドは実行していない。

## 次のIssue

LISS-0098で、non-blocking socketへの部分書き込みとqueue先頭フレームの残りbyte管理を
接続境界へ追加する。

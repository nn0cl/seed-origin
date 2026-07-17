# LISS-0103: server frame処理順序

- Status: review
- Phase: phase-1-red
- Type: architecture + integration
- Priority: critical
- Parent: LISS-0056
- Depends on: LISS-0098, LISS-0100, LISS-0101, LISS-0102

## 目的

サーバーの1フレームを、接続受付、受信、dispatch、応答キューの送信、切断済み接続の
除去の順に実行するAPIとして固定する。accept数をフレームごとに制限し、1回の処理が
無制限に膨張しないようにする。

## 処理順序

1. 最大accept数まで新規ClientSessionを取り込む。
2. 全ClientSessionからnon-blockingにCommandを受信する。
3. 接続IDを保持したままFIFO dispatchする。
4. 各接続のOutboundFrameQueueをflushする。
5. 閉じたClientSessionをSessionRegistryとともに除去する。

## 受入条件

- 停止中は処理せず、0件とエラーを返す。
- acceptの1フレーム上限が定数で保証される。
- NoDataはフレーム失敗にならない。
- 受信したLogin結果を同じ接続の送信キューからflushできる。
- 送信失敗・切断後も他接続の処理を継続する。
- World tick、ActionQueue、Snapshot生成はこのIssueへ混入させない。

## 実装資料

`ServerRuntime::processFrame`を追加し、acceptから切断除去までの処理順序を実装した。
テスト・ビルドは実行していない。

## 次のIssue

固定20FPSの論理tickとこのserver frame APIを接続し、ActionQueueの到着順処理とWorld更新へ
渡す境界を定義する。

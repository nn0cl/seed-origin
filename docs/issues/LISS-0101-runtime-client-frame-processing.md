# LISS-0101: RuntimeのClientSession frame処理

- Status: review
- Phase: phase-1-red
- Type: feature + integration
- Priority: critical
- Parent: LISS-0056
- Depends on: LISS-0094, LISS-0096, LISS-0098, LISS-0100

## 目的

Runtimeの1回の処理で保有ClientSessionからCommandを受信し、接続IDを保持したまま
dispatchする。Login結果は対応するClientSessionのOutboundFrameQueueへLoginResponse
として投入する。

## 受入条件

- 全ClientSessionを走査し、受信できたCommandを内部FIFOへ取り込む。
- Command dispatch時に送信先接続IDを失わない。
- Login成功・拒否の結果を該当接続の送信キューへ入れる。
- NoDataをエラーや切断と扱わない。
- 受信失敗は他接続の処理を停止させず、エラー情報を返す。
- Runtime停止中は処理しない。
- SessionLifecycleによる重複Login拒否、World更新、固定周期イベントループは後続Issueで扱う。

## 実装資料

接続ID付きの内部Commandキューと`processClientFrames`を追加し、LoginResponseを
該当接続のOutboundFrameQueueへ投入する境界を実装した。テスト・ビルドは実行していない。

## 次のIssue

LISS-0099のSessionLifecycleをRuntimeのLogin dispatchへ接続し、重複Login時に新規仮
セッションを残さないようにする。

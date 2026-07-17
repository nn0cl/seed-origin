# LISS-0098: non-blocking outbound write境界

- Status: review
- Phase: phase-1-red
- Type: feature + reliability
- Priority: critical
- Parent: LISS-0095
- Depends on: LISS-0091, LISS-0097

## 目的

接続ごとの送信キューからnon-blocking socketへフレームを書き出す。部分書き込み時は
送信済みbyteだけを先頭から消化し、残りを次回flushへ持ち越す。

## 受入条件

- ClientSessionが受け取ったsocketをnon-blockingとして扱う。
- キュー先頭フレームを送信し、送信済みbyteだけを安全に消化する。
- EAGAIN/EWOULDBLOCKを切断や失敗と誤認しない。
- 書き込みエラー時は接続を閉じ、残留キューを再送可能な状態として扱わない。
- 送信処理がWorld状態、受信Command、SessionRegistryを直接変更しない。
- socket peerへLoginResponseを送れる受入テスト資料がある。

## 非スコープ

再送ポリシー、TLS、送信キューの監視・切断閾値、イベントループ統合は後続Issueで扱う。

## 実装資料

`OutboundFrameQueue`に先頭消化APIを追加し、`ClientSession::flushOutbound`とnon-blocking
socket設定を実装した。テスト・ビルドは実行していない。

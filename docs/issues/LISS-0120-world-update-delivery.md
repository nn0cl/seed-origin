# LISS-0120: WorldUpdate配信境界

- Status: review
- Phase: phase-1-red
- Type: feature + network boundary
- Priority: high
- Parent: LISS-0049
- Depends on: LISS-0117, LISS-0118, LISS-0119, LISS-0056

## 目的

サーバーが確定したSnapshot／Eventを、認証済み接続へ同じsequenceのまま配信する。WorldUpdateのフレーム形式とOutboundFrameQueueの責務を分離し、通信層がWorld状態を直接変更しない境界を維持する。

## 受入条件

- WorldUpdateを専用の識別子付きフレームへエンコード・デコードできる。
- Snapshot／Eventのkind、sequence、worldTick、eventId、payloadを破壊しない。
- 不正magic、予約領域、長さ、WorldUpdate検証違反を拒否する。
- 未認証接続へWorldUpdateを配信しない。
- 認証済み接続のOutboundFrameQueueへFIFOで登録する。
- キュー満杯時に一部配信となったことをエラーとして返し、黙って破棄しない。
- 実際のSocket flush、クライアント描画、再接続再送は後続Issueで扱う。

## 実装資料

`WorldUpdateFrameCodec`と`ServerRuntime::publishWorldUpdates`を追加した。テスト・ビルドは実行していない。

---

## English

### Purpose

Deliver finalized Snapshot and Event updates to authenticated connections without changing their sequence. Keep WorldUpdate framing separate from the outbound queue so the network layer never mutates authoritative world state.

### Acceptance criteria

- Encode and decode WorldUpdate frames with an explicit identity marker.
- Preserve kind, sequence, world tick, event ID, and payload.
- Reject invalid magic, reserved bits, lengths, and invalid WorldUpdate envelopes.
- Do not deliver updates to unauthenticated connections.
- Enqueue updates FIFO in authenticated clients' outbound queues.
- Report partial delivery on queue backpressure instead of silently dropping updates.
- Socket flushing, client rendering, and reconnect replay remain follow-up work.

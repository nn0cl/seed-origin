# LISS-0121: クライアントWorldUpdate受信境界

- Status: review
- Phase: phase-1-red
- Type: feature + client synchronization
- Priority: high
- Parent: LISS-0049
- Depends on: LISS-0118, LISS-0119, LISS-0120

## 目的

TCPの部分受信をWorldUpdate単位へ再構成し、Snapshotとhazard Eventを既存のクライアント状態・EffectQueueへ渡す。受信途中のデータを状態へ適用せず、sequence gapを見逃さない。

## 受入条件

- 1フレームを複数回の受信に分割しても、完全なWorldUpdateだけを適用する。
- 1回の受信に複数WorldUpdateが含まれていても順序を維持する。
- Snapshot適用後、hazard Eventのsequence基準をSnapshotへ合わせる。
- sequence gap、壊れたframe、不正payloadで既存状態とEffectQueueを部分適用しない。
- accumulatorが失敗した後に入力を受け付け続けない。
- Socket、再接続、Snapshot再要求の実送信は後続Issueへ分離する。

## 実装資料

`WorldUpdateAccumulator`と`ClientWorldUpdateReceiver`を追加した。テスト・ビルドは実行していない。

---

## English

### Purpose

Reassemble partial TCP reads into WorldUpdate units and route Snapshots and hazard Events to existing client state and EffectQueue components. Never apply incomplete data and do not miss sequence gaps.

### Acceptance criteria

- Apply only complete WorldUpdates when one frame arrives in multiple reads.
- Preserve order when one read contains multiple updates.
- Align hazard-event sequence tracking after applying a Snapshot.
- Avoid partial state or EffectQueue mutation for sequence gaps, broken frames, or invalid payloads.
- Reject further input after the accumulator enters a failed state.
- Keep sockets, reconnect, and actual snapshot requests as follow-up work.

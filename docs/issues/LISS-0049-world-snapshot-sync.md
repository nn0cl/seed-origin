# LISS-0049: Worldスナップショット・同期

- Status: proposed
- Phase: phase-1-red
- Priority: high
- Parent: LISS-0041
- Depends on: LISS-0023, LISS-0044, LISS-0046, LISS-0047, LISS-0048

接続直後のスナップショット、差分イベント、連番、再送・欠落検出、プレイヤー/NPC/対象物/エーテルの公開範囲を定義する。

## Timing decision

- Server authoritative frames are 20 Hz / 50 ms.
- Snapshot and Event envelopes carry `worldTick` and monotonic sequence values.
- Snapshot is the recovery source of truth; Event is an occurrence stream and is not sufficient for state recovery.
- A client detects a sequence gap and requests resynchronization instead of applying an assumed intermediate state.

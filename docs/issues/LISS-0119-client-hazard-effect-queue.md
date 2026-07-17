# LISS-0119: クライアントhazard EffectQueue

- Status: proposed
- Phase: phase-1-red
- Type: feature + client presentation boundary
- Priority: high
- Parent: LISS-0049
- Depends on: LISS-0116, LISS-0118

## 目的

サーバーのWorldUpdate Eventから環境hazardの演出命令を取り出し、クライアント側のEffectQueueへ渡す。20FPSのサーバー更新と、エフェクトごとの数FPSの消化を分離し、ネットワーク到着順・演出順・状態同期を混同しない。

## 受入条件

- Eventのsequenceを`WorldUpdateSyncController`で検証し、重複は二重演出しない。
- sequence gapはEffectQueueへ投入せず、Snapshot再同期を要求できる。
- `etherHazard=severity:...;instability:...`を有限値として検証する。
- EffectQueueは上限を持ち、満杯時に古い状態同期を破壊しない。
- 1サーバーフレームで到着したEventは到着順に登録する。
- 消化処理は表示フレーム側の責務とし、World状態を直接変更しない。
- hazard以外のEventを黙ってhazard演出へ変換しない。

## 実装方針

まず純粋なEvent変換・FIFO・重複排除・backpressureを実装し、描画APIや実通信への依存を置かない。エフェクトの寿命・優先度・キャンセル規則はクライアントシェルIssueで別途確定する。

---

## English

### Purpose

Convert environment-hazard WorldUpdate events into client EffectQueue commands. Separate the server's 20 FPS updates from per-effect rendering consumption so network order, visual order, and state synchronization are not conflated.

### Acceptance criteria

- Validate event sequences with `WorldUpdateSyncController`; duplicates must not create duplicate effects.
- Do not enqueue events across a sequence gap; allow snapshot resynchronization to be requested.
- Validate `etherHazard=severity:...;instability:...` as finite values.
- Bound the queue without destroying authoritative state synchronization when full.
- Preserve arrival order within one server frame.
- Keep consumption in the presentation layer and never mutate authoritative world state.
- Do not silently convert unrelated events into hazard effects.

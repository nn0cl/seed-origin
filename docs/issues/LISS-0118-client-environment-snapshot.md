# LISS-0118: クライアント環境Snapshot適用境界

- Status: review
- Phase: phase-1-red
- Type: feature + synchronization
- Priority: high
- Parent: LISS-0049
- Depends on: LISS-0117, LISS-0080

## 目的

サーバーが生成した環境Snapshotを、クライアントの状態へ安全に適用する。描画やソケット処理をこの境界へ混ぜず、値の検証・再同期sequenceの更新・状態置換を決定論的に行う。

## 受入条件

- Snapshot以外、eventIdが0でない更新を状態置換に使わない。
- fire／water／earth／air／hazardの全フィールドを一度だけ受け入れる。
- 非有限値、未知フィールド、重複フィールド、欠落フィールドを拒否する。
- hazardは0以上10以下とし、拒否時は既存状態を変更しない。
- 適用後のsequenceを基準に、次の更新sequenceを決定できる。
- 描画、ソケット、ゲームプレイの演出処理は後続Issueへ分離する。

## 実装資料

`ClientEnvironmentState`と`ClientWorldSnapshotApplier`を追加した。Snapshot payloadを厳密に検証してから状態を置換し、sequence trackerをSnapshot後へリセットする。テスト・ビルドは実行していない。

## 次のIssue

環境hazard EventをクライアントのEffectQueueへ変換し、20FPS描画消化へ渡す。

---

## English

### Purpose

Safely apply server-produced environment snapshots to client state. Keep rendering and socket handling outside this boundary so validation, resynchronization sequence updates, and state replacement remain deterministic.

### Acceptance criteria

- Only zero-event Snapshot updates may replace client state.
- All fire, water, earth, air, and hazard fields must occur exactly once.
- Non-finite values, unknown fields, duplicate fields, and missing fields are rejected.
- Hazard must be within 0..10, and rejection must preserve the previous state.
- The next update sequence must be derived from the applied snapshot sequence.
- Rendering, sockets, and gameplay effects remain follow-up work.

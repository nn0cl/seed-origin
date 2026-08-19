# LISS-0122: 再接続Snapshot復旧

- Status: review
- Phase: phase-1-red
- Type: feature + recovery
- Priority: critical
- Depends on: LISS-0121, LISS-0089

## 目的

再接続またはsequence gap後に、クライアントがSnapshotを受け取るまでEventを適用しない。古いWorld状態を新しいEventで部分的に更新する事故を防ぐ。

## 受入条件

- 再接続開始時にSnapshot要求状態へ遷移する。
- Snapshot前のEventを拒否し、既存状態を変更しない。
- Snapshot適用後にEventを再開し、sequence期待値を更新する。
- Snapshot要求状態と期待sequenceを上位の通信層から参照できる。
- 再送・接続確立は後続Issue（LISS-0128）。実際の Snapshot 要求 Command は
  LISS-0154。

## English

After reconnect or a sequence gap, the client must refuse Events until a Snapshot is applied. Expose the pending request and expected sequence to the transport layer. The RequestSnapshot wire Command is LISS-0154; reconnect socket I/O remains LISS-0128.

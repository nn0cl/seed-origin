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
- 再送・接続確立の最小 I/O（POSIX TCP 上の RequestSnapshot）は
  2026-08-19 スライスで接続。タイムアウト・監視・UI は LISS-0128 残。
  実際の Snapshot 要求 Command は LISS-0154。

## 前提の確定（2026-07-22、ADR 0023承認済み）

本Issueの再接続は、30分TTLの正規セッションキーが有効な状態で発生した
一時切断からの復帰とする。正規キーのTTLはクライアント起点のKeep-Aliveで
更新・延長される。チャレンジキーの期限切れや正規キー失効後は再接続ではなく
ネイティブクライアント内のログインから再認証する。

## English

After reconnect or a sequence gap, the client must refuse Events until a Snapshot is applied. Expose the pending request and expected sequence to the transport layer. The RequestSnapshot wire Command is LISS-0154; reconnect socket I/O remains LISS-0128.

Note (2026-07-22): reconnect is gated by the validity of the 30-minute
player session key, with client-driven Keep-Alive extension. This Issue's
acceptance criteria remain focused on Snapshot-before-Event behavior; actual
authentication and session storage are covered by LISS-0146/0147.

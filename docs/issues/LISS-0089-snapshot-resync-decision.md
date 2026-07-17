# LISS-0089: Snapshot再同期判断

- Status: review
- Phase: phase-1-red
- Type: reliability + protocol
- Priority: critical
- Parent: LISS-0084
- Related: LISS-0063, LISS-0080, LISS-0088

## Scope

WorldUpdate sequence trackerの結果を、Snapshot適用、重複無視、Snapshot再取得要求、拒否へ分類する。欠落したEventを推測適用しない。

## Acceptance criteria

- AcceptedはWorldUpdate適用候補として返す。
- Duplicateは副作用なしで無視できる。
- GapはSnapshot再取得要求として返す。
- Gap発生時にsequence trackerを勝手に進めない。
- Invalidは拒否理由を保持する。
- Snapshot受信確認後に次のsequenceから再開できる。
- 実際の再送・transport送信は別adapterの責務にする。

再同期判断Controllerを実装した。Snapshotの実データ適用とtransport送信は後続Issueで扱う。テスト・ビルドは運用規約により実行していない。

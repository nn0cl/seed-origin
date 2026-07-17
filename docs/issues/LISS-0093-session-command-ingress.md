# LISS-0093: ClientSessionからServerRuntimeへのCommand取り込み

- Status: review
- Phase: phase-1-red
- Type: network + architecture
- Priority: critical
- Parent: LISS-0056
- Related: LISS-0073, LISS-0091, LISS-0070

## Scope

ClientSessionが生成した検証済みCommand列を、ServerRuntimeの有界FIFOへ取り込む。取り込み境界はCommandをqueueへ渡すだけで、Login、World、Snapshotのビジネスロジックを持たない。

## Acceptance criteria

- ServerRuntime停止中のsession入力を受け付けない。
- ClientSessionのReceiveStatusを維持して返す。
- 1回のreadで生成された複数Commandを順序どおり取り込む。
- queue容量不足時は取り込み前に拒否し、部分取り込みを起こさない。
- Command検証失敗時はqueueを変更しない。
- Login処理・World変更・応答writeは後続の責務とする。

ClientSessionから有界Command queueへの取り込みを実装した。テスト・ビルドは運用規約により実行していない。

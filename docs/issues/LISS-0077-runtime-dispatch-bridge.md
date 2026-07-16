# LISS-0077: RuntimeからCommand Dispatcherへの接続

- Status: review
- Phase: phase-1-red
- Type: architecture
- Priority: critical
- Parent: LISS-0058
- Related: LISS-0056, LISS-0073, LISS-0076

## Scope

`ServerRuntime`の有界FIFOから取り出したCommandを`ServerCommandDispatcher`へ渡し、結果を同じ順序で返す接続を定義する。Runtimeは個別ゲーム機能やWorldの所有者にならない。

## Acceptance criteria

- pending CommandをFIFO順にDispatcherへ渡す。
- dispatch後はRuntimeのpending queueを空にする。
- Dispatcherの結果順序と拒否理由をそのまま呼び出し元へ返す。
- RuntimeにsocketやWorldのビジネスロジックを追加しない。
- テスト資料を追加する。

実装とテスト資料を追加した。テスト・ビルドは運用規約により実行していない。

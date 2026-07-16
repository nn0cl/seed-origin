# LISS-0076: Server Command dispatch境界

- Status: review
- Phase: phase-1-red
- Type: architecture + feature
- Priority: critical
- Parent: LISS-0058
- Related: LISS-0056, LISS-0057, LISS-0073

## Scope

`ServerRuntime`が受け付けたCommandを、Command種別ごとのサーバー側ハンドラーへ渡す境界を追加する。最初の実装対象はLoginのみとし、未実装のMove/Chat/Attack/CastSpell/Disconnectは暗黙に通さず拒否する。

## Acceptance criteria

- Login Commandが`LoginCommandHandler`へ渡され、内部IDを持つ一時Sessionが返る。
- 未実装Commandは拒否され、理由を結果に保持する。
- 複数Commandの入力順序を維持して結果を返す。
- DispatcherはWorldやsocketの所有権を持たない。
- テスト資料を追加する。

実装とテスト資料を追加した。テスト・ビルドは運用規約により実行していない。

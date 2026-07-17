# LISS-0091: ClientSessionとFrameAccumulator接続

- Status: review
- Phase: phase-1-red
- Type: network + safety
- Priority: critical
- Parent: LISS-0056
- Related: LISS-0075, LISS-0090, LISS-0057

## Scope

受理済みclient socketをClientSessionが所有し、非ブロッキングreadの入力を接続ごとのFrameAccumulatorへ渡して、検証済みNetworkCommandだけをServerRuntimeへ返す。

## Acceptance criteria

- ClientSessionがclient socketの所有権を持ち、破棄時に一度だけcloseする。
- 部分readはFrameAccumulatorへ渡し、完全frameになるまでCommandを返さない。
- 複数frameの順序を保持する。
- データなし、Command生成、正常切断、read失敗を区別する。
- decode失敗や上限超過時にsessionを失敗状態として閉じる。
- ClientSessionはLoginやWorld状態を直接処理しない。

ClientSessionとFrameAccumulatorの接続を実装した。write、Command dispatch、login応答は後続スライスで扱う。テスト・ビルドは運用規約により実行していない。

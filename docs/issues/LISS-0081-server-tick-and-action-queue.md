# LISS-0081: Server tickとActionQueue実装

- Status: review
- Phase: phase-1-red
- Type: feature + determinism
- Priority: critical
- Parent: LISS-0079
- Related: LISS-0022, LISS-0023, LISS-0058

## Scope

20Hz固定ステップ、受付シーケンス、フレーム開始時のqueue swap、到着順Action適用、処理中到着分の次フレーム繰り越しを実装する。

## Acceptance criteria

- 1フレームを50msとして処理する。
- 同一フレームのActionを受付順に一度だけ処理する。
- 処理中に到着したActionが現在の処理へ割り込まない。
- 停止・満杯・空キューを安全に扱う。
- queueの所有権が明確でraw pointerを保持しない。
- 固定入力列の再実行結果が一致する。

値を保持するActionQueue、受付sequence、mutexによる受付境界、`takeFrame`によるフレーム切り替えを実装した。固定時計とWorld適用は後続スライスで扱う。テスト・ビルドは運用規約により実行していない。

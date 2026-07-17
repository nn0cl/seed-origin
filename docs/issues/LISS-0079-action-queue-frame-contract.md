# LISS-0079: 20 FPS ActionQueueフレーム契約

- Status: review
- Phase: phase-0-design
- Type: architecture + feature
- Priority: critical
- Parent: LISS-0023
- Related: LISS-0022, LISS-0026, LISS-0058

## Scope

複数ユーザーから到着したActionを、サーバーの20 FPS固定フレームで確定し、到着順に一度ずつ処理する仕組みを定義・実装する。現在の`include/seed/ActionQueue.h`はraw pointerの保持だけであり、ActionQueue本体としては未実装である。

## Acceptance criteria

- 1フレームを50msの固定ステップとして扱う。
- フレーム開始時に、受付済みActionを処理用キューへスワップする。
- 同一フレームに到着したActionをサーバー受付シーケンス順に処理する。
- フレーム処理中に到着したActionは次フレームへ繰り越す。
- Actionを所有するキューはraw pointerを保持せず、値または明確な所有スマートポインターを使用する。
- 空キュー、満杯、停止中の受付を安全に扱う。
- 同じ入力列・到着順・初期World状態から同じ結果を得る。
- 並行受付と固定フレーム処理の排他境界をテスト資料化する。

実装は後続スライスで行う。テスト・ビルドは運用規約により実行しない。

LISS-0081で、値保持・受付sequence・mutex境界・フレーム切り替え用`takeFrame`を実装した。固定時計とWorldへの適用は後続スライスで扱う。

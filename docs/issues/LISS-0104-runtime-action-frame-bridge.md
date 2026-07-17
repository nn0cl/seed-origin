# LISS-0104: Runtimeと20FPS ActionQueueの接続

- Status: review
- Phase: phase-1-red
- Type: architecture + deterministic simulation
- Priority: critical
- Parent: LISS-0056
- Depends on: LISS-0079, LISS-0081, LISS-0086, LISS-0103

## 目的

server frameごとに論理World tickを1つ進め、ActionQueueへ到着したActionをそのフレームの
処理単位として切り出す。実時間のsleepやWorldへの副作用はRuntimeへ入れず、呼び出し側が
50ms（20FPS）周期でこのAPIを呼ぶ。

## 受入条件

- 稼働中のRuntimeへActionを投入できる。
- `processFrame`ごとにworldTickが1増加する。
- フレーム開始前に到着したActionをsequence順で一括取得する。
- フレーム切り出し後に到着したActionは次フレームへ残る。
- Runtime停止中はActionを受け付けず、tickを進めない。
- Actionの適用、Field変更、魔法・攻撃・エーテル減衰は後続Issueで扱う。

## 実装資料

ServerRuntimeへActionQueue／ServerTickを所有させ、`ServerFrameResult`として論理tickと
フレームActionを返す境界を追加した。20FPSの実時間スケジューラは未実装である。
テスト・ビルドは実行していない。

## 次のIssue

FrameActionsをWorld更新器へ渡し、Actionの決定論的な適用結果をWorldUpdateへ変換する。

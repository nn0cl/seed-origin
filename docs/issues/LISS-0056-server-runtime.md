# LISS-0056: ゲームサーバー実行ループ

- Status: review
- Phase: phase-1-red
- Priority: critical
- Parent: LISS-0041
- Depends on: LISS-0023, LISS-0054, LISS-0055

リスナー、接続受付、Commandキュー、固定時間World更新、スナップショット送信、停止・再起動を統合する。通信スレッドがWorld状態を直接変更しない。

Active slice: LISS-0073, LISS-0090, LISS-0091, LISS-0093, LISS-0094, LISS-0100, LISS-0101, LISS-0102, LISS-0103, LISS-0104, LISS-0105, LISS-0106, LISS-0107。accept、ClientSession/frame境界、Command取り込み、Login dispatch、ClientSession所有、LoginResponse投入、SessionLifecycle接続、server frame順序、ActionQueue／logical tick接続、WorldUpdate Event変換、Field適用、移動入力queueを追加した。MovementIntent適用、攻撃・魔法、snapshot送信は後続スライスで実装する。

# LISS-0056: ゲームサーバー実行ループ

- Status: proposed
- Phase: phase-1-red
- Priority: critical
- Parent: LISS-0041
- Depends on: LISS-0023, LISS-0054, LISS-0055

リスナー、接続受付、Commandキュー、固定時間World更新、スナップショット送信、停止・再起動を統合する。通信スレッドがWorld状態を直接変更しない。

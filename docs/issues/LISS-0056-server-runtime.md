# LISS-0056: ゲームサーバー実行ループ

- Status: review
- Phase: phase-1-red
- Priority: critical
- Parent: LISS-0041
- Depends on: LISS-0023, LISS-0054, LISS-0055

リスナー、接続受付、Commandキュー、固定時間World更新、スナップショット送信、停止・再起動を統合する。通信スレッドがWorld状態を直接変更しない。

Active slice: LISS-0073, LISS-0090, LISS-0091, LISS-0093, LISS-0094, LISS-0100, LISS-0101, LISS-0102, LISS-0103, LISS-0104, LISS-0105, LISS-0106, LISS-0107, LISS-0108, LISS-0109, LISS-0110, LISS-0111, LISS-0112, LISS-0113, LISS-0114, LISS-0115, LISS-0116, LISS-0117, LISS-0118, LISS-0119, LISS-0120, LISS-0121, LISS-0122。accept、ClientSession/frame境界、Command取り込み、Login dispatch、ClientSession所有、LoginResponse投入、SessionLifecycle接続、server frame順序、ActionQueue／logical tick接続、WorldUpdate Event変換、Field適用、移動入力queue、MovementIntent適用、統合入力queue、Runtime／WorldFrameApplier移行、統合入力Event化とField順序適用、攻撃・魔法Intent入力、Combat／Spell対象・距離・威力適用、環境エーテル・伝導率・減衰、属性間相互作用・hazard観測、hazard Event同期、環境Snapshot集約、クライアント環境Snapshot適用、クライアントhazard EffectQueue、WorldUpdate配信境界、クライアントWorldUpdate受信境界、再接続Snapshot復旧を追加した。認証なし名寄せ、チャット、3D移動、NPC、攻撃・魔法の公開Command、クライアント通信・描画、レート制限、受入試験は後続Issueで実装する。

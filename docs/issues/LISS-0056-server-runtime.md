# LISS-0056: ゲームサーバー実行ループ

- Status: review
- Phase: phase-1-red
- Priority: critical
- Parent: LISS-0041
- Depends on: LISS-0023, LISS-0054, LISS-0055

リスナー、接続受付、Commandキュー、固定時間World更新、スナップショット送信、停止・再起動を統合する。通信スレッドがWorld状態を直接変更しない。

Active slice: LISS-0073, LISS-0090, LISS-0091, LISS-0093, LISS-0094, LISS-0100, LISS-0101, LISS-0102, LISS-0103, LISS-0104, LISS-0105, LISS-0106, LISS-0107, LISS-0108, LISS-0109, LISS-0110, LISS-0111, LISS-0112, LISS-0113, LISS-0114, LISS-0115, LISS-0116, LISS-0117, LISS-0118, LISS-0119, LISS-0120, LISS-0121, LISS-0122, LISS-0124, LISS-0125, LISS-0126, LISS-0138。accept、ClientSession/frame境界、Command取り込み、Login dispatch、ClientSession所有、LoginResponse投入、SessionLifecycle接続、server frame順序、ActionQueue／logical tick接続、WorldUpdate Event変換、Field適用、移動入力queue、MovementIntent適用、統合入力queue、Runtime／WorldFrameApplier移行、統合入力Event化とField順序適用、攻撃・魔法Intent入力、Combat／Spell対象・距離・威力適用、環境エーテル・伝導率・減衰、属性間相互作用・hazard観測、hazard Event同期、環境Snapshot集約、クライアント環境Snapshot適用、クライアントhazard EffectQueue、WorldUpdate配信境界、クライアントWorldUpdate受信境界、再接続Snapshot復旧、チャット入力境界、権威3D移動境界、攻撃・魔法Command接続、Combat request ID冪等性境界、WorldTick基準のMP・クールダウン境界を追加した。認証なし名寄せ、NPC、クライアント通信・描画、レート制限、受入試験は後続Issueで実装する。

## Composition root（2026-07-18追加）

上記Active sliceは`ServerRuntime`／`ServerCommandDispatcher`／`WorldFrameApplier`と
してライブラリ単位では実装済みだったが、それらを実際に束ねて起動する`main()`が
存在しなかった。`src/ServerMain.cpp`（`seed_server`実行ファイル）を追加し、以下を
合成した。

- port引数（既定40000）で`ServerRuntime::start`。
- SIGINT/SIGTERMで`g_shutdownRequested`を立て、ループを抜けて
  `runtime.stop(registry)`する。
- 固定tick（50ms、20FPS。既存Canonical実装が前提とする入力tick間隔を踏襲）で
  `processFrame` → `WorldFrameApplier::apply` → `publishWorldUpdates`のループ。
- `IdentityAliasStore`の選択は`SEED_IDENTITY_DB_URL`の有無で
  `PostgresIdentityAliasStore`／`InMemoryIdentityAliasStore`を切り替える
  （LISS-0143、ADR 0016）。`SEED_HAVE_LIBPQXX`未定義（libpqxx未検出）の
  ビルドではPostgres分岐自体がコンパイル対象外になる。

Field::getInstance()の既存シングルトン設計は今回変更していない。認証・
レート制限のTLS/証明書設定、実運用のport/設定ソース選定は未着手のまま。
警告有効C++20ビルドを実行して確認した。Adjudicator方針によりサーバー起動
・テスト実行は未実施。

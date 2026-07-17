# LISS-0102: RuntimeへのSessionLifecycle統合

- Status: review
- Phase: phase-1-red
- Type: feature + security
- Priority: critical
- Parent: LISS-0056
- Depends on: LISS-0099, LISS-0100, LISS-0101

## 目的

RuntimeのLogin dispatchとSessionLifecycleを接続し、同一接続の重複Loginを処理前に
拒否する。成功したSessionInfoだけを接続へ束縛し、閉じたClientSessionの除去時に
SessionRegistryからlogoutする。

## 受入条件

- 同一接続に既存セッションがある場合、LoginCommandをSessionRegistryへ再送しない。
- 初回Login成功時だけSessionLifecycleへ束縛する。
- 束縛失敗時に新規セッションをSessionRegistryへ残さない。
- 閉じたClientSessionの除去時に束縛済みセッションをlogoutする。
- Runtime停止時に、registryを渡した停止経路では束縛済みセッションを全てlogoutできる。
- 認証、永続化、再接続時のセッション復元は扱わない。

## 実装資料

RuntimeのLogin dispatch前重複検査、成功結果の束縛、切断済みClientSession除去時の
logout、registry付き停止経路を追加した。テスト・ビルドは実行していない。

## 次のIssue

固定周期のserver frame APIを追加し、accept・受信・dispatch・送信・切断処理を1回の
サーバーフレームとして順序付ける。

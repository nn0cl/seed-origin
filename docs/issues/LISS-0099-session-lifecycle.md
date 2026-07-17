# LISS-0099: セッションライフサイクルと重複Login

- Status: review
- Phase: phase-1-red
- Type: feature + security
- Priority: critical
- Parent: LISS-0095
- Depends on: LISS-0053, LISS-0072, LISS-0094, LISS-0098

## 目的

接続と仮セッションの対応を明示し、同一接続への重複Loginを拒否する。接続終了時には
束縛されたセッションをSessionRegistryからlogoutし、切断後に古い内部IDを使い続けられ
ない境界を作る。

## 受入条件

- 接続IDごとに高々一つのSessionInfoを束縛できる。
- 不正な接続IDまたはセッションIDを束縛しない。
- 既に束縛済みの接続へのLoginを拒否し、既存の束縛を変更しない。
- disconnectでSessionRegistryのactive状態を解除し、束縛を削除する。
- 未束縛接続のdisconnectは安全に失敗する。
- 認証、永続化、再接続時のID復元、複数デバイス方針は混入させない。

## 実装資料

`SessionLifecycle`として接続IDと内部セッションIDの束縛、重複Login拒否、切断時logoutを
追加した。Runtimeの接続コレクションへの組み込みは後続Issueで行う。テスト・ビルドは
実行していない。

## 次のIssue

ServerRuntimeがacceptしたClientSessionを保持し、受信・dispatch・送信・切断処理を固定
周期で実行する接続コレクションを追加する。

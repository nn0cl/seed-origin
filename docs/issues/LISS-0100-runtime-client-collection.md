# LISS-0100: ServerRuntimeのClientSessionコレクション

- Status: review
- Phase: phase-1-red
- Type: architecture + feature
- Priority: critical
- Parent: LISS-0056
- Depends on: LISS-0090, LISS-0091, LISS-0098, LISS-0099

## 目的

ServerRuntimeがacceptしたClientSessionを所有し、接続IDを発行して後続の受信・dispatch・
送信処理から参照できるようにする。接続の所有権をRuntimeへ集約し、停止時に全接続を
安全に閉じる。

## 受入条件

- 停止中のacceptを拒否し、接続IDを発行しない。
- accept成功時に0以外の単調増加する接続IDを発行する。
- RuntimeがClientSessionの所有権を持ち、外部へ所有権を渡さない。
- stopで保有ClientSessionとpending Commandを破棄する。
- 既に閉じたClientSessionをコレクションから除去できる。
- 受信・dispatch・送信の周期処理、SessionLifecycleへのLogin／logout接続は後続Issueで扱う。

## 実装資料

RuntimeへClientSessionの所有コレクション、接続ID、accept／参照／切断済み除去APIを
追加した。テスト・ビルドは実行していない。

## 次のIssue

Runtimeの1フレーム処理で全ClientSessionを走査し、受信CommandをFIFOへ取り込み、
dispatch結果をLoginResponseとして該当接続の送信キューへ戻す。

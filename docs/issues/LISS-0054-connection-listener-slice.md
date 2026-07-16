# LISS-0054: Connectionリスナー寿命・ポート契約スライス

- Status: review
- Phase: phase-1-red
- Type: safety + architecture
- Priority: high
- Parent: LISS-0029
- Related: LISS-0044

## Scope

リスナーソケットをConnectionオブジェクトが所有し、指定ポートでbind/listen後に保持する。closeを明示的かつ冪等にし、接続処理・Command decodeは別スライスへ分離する。

## Acceptance criteria

- 固定ポート12345をAPI内部で使用しない。
- bind/listen成功後にソケットを即時closeしない。
- `closeSocket`でソケットを閉じ、デストラクタでもリークしない。
- open/closeの成否を戻り値で取得できる。
- 受信処理、認証、フレームdecode、world変更は本スライスに追加しない。

実装とテスト資料を追加した。configure/build/testと実ソケット接続試験は実行していない。

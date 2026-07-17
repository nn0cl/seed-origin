# LISS-0053: 仮セッション・ID名寄せレジストリスライス

- Status: review
- Phase: phase-1-red
- Type: feature + safety
- Priority: high
- Parent: LISS-0042
- Related: LISS-0051

## Scope

認証なしログインのサーバー内契約を実装する。セッション内部IDはログインごとに採番し、申告IDには再起動前の名寄せIDを割り当てる。永続化はLISS-0043で扱う。

## Acceptance criteria

- クライアントが内部IDを指定できない。
- 同じ有効な申告IDはASCII大小文字を正規化した同じalias IDになる。
- ログインごとに異なる内部セッションIDになる。
- 空・長すぎる・allowlist外の申告IDは匿名扱いになる。
- 切断した内部IDは再利用されない。
- 認証済みを示す状態を返さない。

## Verification note

実装とテスト資料を追加した。永続化・通信接続・レート制限は未実装。コンパイル・テストは運用規約により実行していない。

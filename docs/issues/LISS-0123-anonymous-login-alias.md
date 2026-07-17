# LISS-0123: 匿名ログインと申告ID名寄せ

- Status: proposed
- Priority: high
- Depends on: LISS-0042, LISS-0053, LISS-0066

## 目的

認証なしで匿名セッションを作り、内部IDをサーバーで自動採番する。申告ユーザーIDは表示名・再接続ヒントとして扱い、過去IDとの名寄せは明示的なポリシーと衝突防止を経て行う。

## 受入条件

- クライアントは内部IDを指定できない。
- 内部IDは単調性・再利用禁止を満たす。
- 申告IDと内部IDを混同せず、個人認証済みと表示しない。
- 名寄せ候補の衝突、空文字、予約語、長さ、Unicode正規化を処理する。
- 永続化・削除・漏洩・なりすまし対策を仕様化するまで本番利用しない。

## English

Create anonymous sessions with server-assigned internal IDs. Treat a claimed user ID as an alias or reconnect hint, never as authentication, and define collision, privacy, persistence, and impersonation rules before production use.

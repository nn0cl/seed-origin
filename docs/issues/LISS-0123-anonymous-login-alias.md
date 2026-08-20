# LISS-0123: 匿名ログインと申告ID名寄せ

- Status: superseded
- Superseded by: ADR 0018 / LISS-0147 / LISS-0150（2026-08-20）
- Priority: high
- Depends on: LISS-0042, LISS-0053, LISS-0066

## 目的

認証なしで匿名セッションを作り、内部IDをサーバーで自動採番する。申告ユーザーIDは表示名・再接続ヒントとして扱い、過去IDとの名寄せは明示的なポリシーと衝突防止を経て行う。

## 受入条件

- クライアントは内部IDを指定できない。
- 内部IDは単調性・再利用禁止を満たす。
- 申告IDと内部IDを混同せず、個人認証済みと表示しない。
- 申告IDは許可文字・長さを検証し、ASCII大小文字を正規化して同じaliasへ名寄せする。
- 名寄せ候補の衝突、空文字、予約語、長さ、Unicode正規化を処理する。
- 永続化・削除・漏洩・なりすまし対策を仕様化するまで本番利用しない。

## 実装資料

`SessionRegistry`で内部IDを自動採番し、認証状態を常にfalseの匿名セッションとして保持する。申告IDは形式検証後にASCII大小文字を正規化して一時aliasへ名寄せする。永続化はLISS-0130で扱う。テスト・ビルドは実行していない。

## Supersession（LISS-0150、2026-08-20）

- プレイヤー認証は登録制（ADR 0018）へ移行済み。匿名 claimed-ID Login は
  LISS-0147（PR #12）で削除された。
- 本Issueは削除せず `superseded` として残す。
- `IdentityAliasStore` コードの削除タイミングは LISS-0150 / ADR 0023 決定5参照。

## English

Superseded by registered-player authentication (ADR 0018). Anonymous login
and claimed-ID aliasing are no longer the player identity path; see LISS-0150.

# LISS-0147: ワールドサーバーのセッショントークン検証ログインへの置換

- Status: proposed
- Phase: phase-0-design-intake
- Priority: high
- Depends on: LISS-0146
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

`seed_server`の`Login` Commandを、自己申告ニックネームの無条件受理から、
`seed_auth`が発行したセッショントークンの検証（`player_sessions`照合）へ
置き換える。

## 受入条件（ドラフト）

- `NetworkCommand::Login`のpayloadはセッショントークンとして扱う。
- `seed_server`は`player_sessions`（Postgres）を照合し、`user_id`を
  解決する。パスワードや`users`テーブルには一切触れない。
- 無効・期限切れトークンはログイン拒否とする。
- `SessionRegistry::login(claimedId)`の「任意の整形済みニックネームを
  無条件で受理する」現行動作は削除する（死んだコードとして残さない）。
- 複数`seed_server`インスタンスが同じセッショントークンを検証できる
  （in-memoryに依存しない）ことをPostgres共有で確認する。

## 設計課題

- `LoginCommandHandler`／`SessionRegistry`をどこまで作り替えるか
  （新規`PostgresPlayerSessionValidator`ポートを追加するか、既存クラスを
  改修するか）。
- LISS-0122（5分間再接続Snapshot復旧）との整合：再接続時にセッション
  トークンをどう扱うか。

## Remaining decisions

- 上記はLISS-0146の設計確定後に着手する。

## English

Design intake for replacing seed_server's anonymous claimed-ID login with
Postgres-backed session-token validation against player_sessions, resolving
a user_id without the world server ever seeing credentials.

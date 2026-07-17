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

- `NetworkCommand::Login`のpayloadは`seed_auth`発行の**初期セッションキー**
  として扱う。
- `seed_server`は`player_sessions`（Postgres）を照合し、`user_id`を
  解決する。パスワードや`users`テーブルには一切触れない。
- 無効・期限切れ・**既に使用済み（`claimed_at IS NOT NULL`）**トークンは
  ログイン拒否とする。
- `SessionRegistry::login(claimedId)`の「任意の整形済みニックネームを
  無条件で受理する」現行動作は削除する（死んだコードとして残さない）。
- 複数`seed_server`インスタンスが同じセッショントークンを検証できる
  （in-memoryに依存しない）ことをPostgres共有で確認する。

## セッションキーの「先勝ち」ローテーション（2026-07-18、Adjudicator決定）

- クライアントは`seed_auth`の`/login`で得た初期セッションキーで
  `seed_server`へログインする。
- `seed_server`はログイン成功時、そのキーを**使用済みにし、新しい
  プレイ継続用キーを発行**する。SQLで表すと概ね次の形（楽観ロックと
  同じ発想）:
  ```sql
  UPDATE player_sessions
  SET claimed_at = now(), session_token = <new_token>
  WHERE session_token = <presented_token>
    AND claimed_at IS NULL
    AND expires_at > now()
  RETURNING user_id;
  ```
  0件更新なら「無効・期限切れ・既に他の接続が同じキーを使用済み」の
  いずれかとしてログイン拒否する。同じキーで後から来た2件目のログイン
  試行は必ず失敗する（先勝ち）。
- 名寄せ（`identity_aliases`）由来の競合ユースケースはADR 0018により
  そもそも存在しない（申告IDという概念自体が廃止されるため）。今回の
  「先勝ちキー消費」が、登録制認証モデルにおける唯一の競合防止機構となる。
- LISS-0122（5分間再接続）は、ワールドログイン成功時に発行された
  **ローテーション後の新キー**を使って再接続する前提とする。初期キー
  （`seed_auth`発行のもの）は一度きりで再接続には使えない。

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

# LISS-0146: `users`テーブルと`seed_auth`（登録・ログイン・Postgresセッション）

- Status: proposed
- Phase: phase-0-design-intake
- Priority: high
- Depends on: なし（ADR 0018が起点）
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

登録制プレイヤー認証の基盤となる`users`テーブルと、資格情報検証・
セッション発行を担う`seed_auth`実行ファイルを追加する。

## 受入条件（ドラフト）

- `users`テーブル（`id`, `username` unique, `password_hash`）を追加する。
- `player_sessions`テーブル（`session_token`, `user_id`, `created_at`,
  `expires_at`, `claimed_at`）をPostgresに追加し、`AdminSessionStore`と
  異なり**in-memoryではなくPostgresに保存する**（複数`seed_server`
  インスタンスが同じセッション情報を参照できるようにするため）。
  `claimed_at`は「先勝ち」ログインの実現に使う（LISS-0147参照）。
- `POST /register`：ユーザー名・パスワードを受け取り`users`へ`pgcrypto`
  ハッシュで登録する。
- `POST /login`：資格情報を`pgcrypto`の`crypt()`で検証し、
  `player_sessions`へ**ログイン専用の初期セッションキー**を発行する
  （ワールドサーバーへの実際のログインで使う一回限りのキー。詳細な
  ライフサイクルはLISS-0147参照）。
- `POST /logout`：セッションを失効させる。
- `seed_auth`は`seed_admin`・`seed_server`と別プロセス・別実行ファイルと
  する。

## ディレクトリ構造・Dockerネットワーク（2026-07-18確定）

```
seed-auth/
├── docker-compose.yml
├── backend/            # 実装言語は下記「実装言語」参照、未確定
│   └── ...
└── frontend/           # LISS-0149のスコープ
    └── src/...
```

`seed-admin`と同じ外部共有Dockerネットワーク（`seed-network`、
LISS-0145参照）に参加する。`db/docker-compose.yml`・
`seed-admin/docker-compose.yml`・`seed-auth/docker-compose.yml`が
共通の`seed-network`を参照する構成。

## 資格情報の流通境界（2026-07-18、Adjudicator確認済み）

パスワード（および`pgcrypto`ハッシュ）は`seed_auth`とゲームクライアント
（またはその手前のWeb登録・ログイン画面、LISS-0149）の間でのみやり取り
する。`seed_server`とクライアントの間、`seed_server`と`seed_auth`の間は
**一回限りのトークンのみ**が流れ、パスワード・ハッシュは一切渡さない
（LISS-0147の「先勝ちセッションキーローテーション」参照）。

## 実装言語（2026-07-18時点で未確定）

ADR 0019により`seed_admin`はKotlin + Spring Boot + Reactへ移行する方針と
なった。`seed_auth`も同じ理由（開発速度）で同一スタックへ揃える可能性が
高いが、ADR 0019では明示的に決定していない。本Issueの実装言語は
`seed_auth`着手前に別途確定させる（C++/libpqxx+cpp-httplibパターンの
記述は撤回し、未決定として扱う）。

## 設計課題（Must not guess、次のAI/Adjudicator協議で確定させる）

- 登録時のユーザー名重複・禁止文字・長さ制限のポリシー。
- セッショントークンのTTL・再接続猶予との整合（LISS-0122の5分間再接続との
  関係、ADR 0018のambiguity参照）。
- ブルートフォース対策（`AdminLoginLockout`と同様の仕組みを流用するか）。
- `seed_auth`と`seed_admin`のセッションストア実装を共通化するか
  （`AdminSessionStore`をPostgres版に拡張して両者から使うか、独立させるか）。

## Remaining decisions

- 上記設計課題はすべて未決定。Phase 1（Red）着手前にAdjudicator確認が
  必要。

## English

Design intake for the `users` table, a Postgres-backed `player_sessions`
table, and a new `seed_auth` executable (registration, login, logout) per
ADR 0018. No implementation has started; open design questions (token TTL,
lockout policy, session-store code sharing with seed_admin) are listed
above.

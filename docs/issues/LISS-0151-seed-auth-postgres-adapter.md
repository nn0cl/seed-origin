# LISS-0151: `seed_auth` PostgreSQL / pgcrypto Adapter

- Status: done
- Phase: done
- Related branch: merged via PR #16 (`feature/liss-0151-seed-auth-postgres-adapter`)
- Priority: high
- Depends on: LISS-0146（UseCase done）
- Parent: LISS-0146
- Related ADR: `docs/architecture/adr/0023-player-auth-session-flow-details.md`
- Blocks: LISS-0149（実用的には HTTP Adapter も必要）, seed_auth delivery

## 目的

LISS-0146 UseCase のポート実装として、PostgreSQL + `pgcrypto` を使う
Adapter を追加する。HTTP / Spring Boot は含めない（次スライス）。

## スコープ

- `UserStore` → Postgres `users`
- `PlayerChallengeStore` → Postgres `player_challenges`
- `PlayerSessionStore` → Postgres `player_sessions`（`revoke`）
- `PasswordCodec` → `pgcrypto`（`crypt()` / `gen_salt('bf')`）
- 統合テスト（`SEED_IDENTITY_DB_URL` 未設定時は skip）
- 既存 migration `db/migrations/0003_player_challenges_sessions.sql` を正とする

## スコープ外

- Spring Boot HTTP（`POST /register` `/login` `/logout`）— **LISS-0155**
- ロックアウトの Postgres 永続化（UseCase のプロセス内メモリを維持）
- Keep-Alive / セッション発行（LISS-0147 / `seed_server`）

## 受入条件（Adapter A）

- Given DB URL が設定されている
- When ユーザーを保存しパスワードを `pgcrypto` でハッシュする
- Then `users` 行が作成され、平文パスワードは保存されない

- Given 登録済みユーザー
- When 正しいパスワードで `matches` する
- Then true を返す

- Given `PlayerChallengeStore.issue`
- When challenge を INSERT する
- Then `player_challenges` に未 claim 行が存在する

- Given active `player_sessions` 行
- When `PlayerSessionStore.revoke`
- Then `revoked_at` がセットされる

## Phase 1 Red（2026-08-20）

- Tests: `seed-auth/backend/src/test/kotlin/com/seed/auth/adapter/postgres/PostgresAuthAdapterTest.kt`
- Covered: user save/find + pgcrypto match; challenge issue; session revoke
- Expected Red: compile failure（`PostgresUserStore` 等未実装）
- Out of Red: Spring HTTP, MyBatis バージョン固定の本番配線（Green で最小選定）

## Phase 2 Green（2026-08-20）

- Adapters: `PgcryptoPasswordCodec`, `PostgresUserStore`,
  `PostgresPlayerChallengeStore`, `PostgresPlayerSessionStore`
- Persistence: JDBC + `org.postgresql:postgresql`（DriverManager）
- MyBatis: Spring HTTP Adapter スライスへ延期（ADR 0019/0023 の本番配線と同時）
- 接続: `SEED_IDENTITY_DB_URL`（libpq URL → JDBC URL + Properties）
- Verification: `SEED_IDENTITY_DB_URL` 付きで `PostgresAuthAdapterTest` 3/3 PASSED
- Adjudicator: **承認**（2026-08-20）— Green + JDBC 採用 / MyBatis 延期を含む

## Phase 3 Refactor（2026-08-20）

- JDBC の prepare/execute ネストを `query` / `update` ヘルパーへ集約
- SQL・ポート契約・受入アサーションは変更なし
- Adjudicator: Phase 3 実施後 commit / PR 承認

## Completion（2026-08-20）

- Merged via PR #16 (`5ed266c`).
- Follow-up HTTP / Spring Boot Adapter: **LISS-0155**.

## English

Add PostgreSQL + pgcrypto adapters for seed_auth ports from LISS-0146.
HTTP/Spring Boot is a follow-up slice (LISS-0155).

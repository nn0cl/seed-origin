# LISS-0155: `seed_auth` Spring Boot HTTP Adapter

- Status: done
- Phase: done
- Related branch: merged via PR #18 (`feature/liss-0155-seed-auth-http-adapter`)
- Priority: high
- Depends on: LISS-0151（Postgres/pgcrypto Adapter done — PR #16）
- Parent: LISS-0146
- Related ADR: `docs/architecture/adr/0023-player-auth-session-flow-details.md`,
  `docs/architecture/adr/0019-admin-backend-language-kotlin-spring-boot.md`
- Blocks: LISS-0149（登録 SPA）, seed_auth delivery

## 目的

LISS-0146 UseCase と LISS-0151 Postgres Adapter を、Spring Boot HTTP
デリバリ（`POST /register` `/login` `/logout`）で公開する。ADR 0019/0023
の MyBatis 配線を本スライスで導入する。

## スコープ

- Spring Boot アプリケーション起動（`seed-auth/backend`）
- HTTP Adapter: `POST /register`, `POST /login`, `POST /logout`
- UseCase（`PlayerAuthenticationService`）への委譲のみ（業務判断は Adapter に置かない）
- MyBatis（または MyBatis + 既存 JDBC の段階移行）によるポート配線
- `SEED_IDENTITY_DB_URL` からの DataSource
- MockMvc 契約テスト（UseCase はモック；DB 不要）

## スコープ外

- LISS-0149 React SPA
- ロックアウトの Postgres 永続化（プロセス内メモリ維持）
- `seed_server` Keep-Alive / challenge claim（LISS-0147）
- パスワードリセット API（将来）
- MyBatis mapper による JDBC 全置換（段階導入の次段）

## 受入条件（LISS-0146 API シナリオの HTTP 面）

- Given 未登録ユーザー名と有効パスワード
- When `POST /register`
- Then 201 相当でユーザー作成、平文パスワードは保存されない

- Given 既存ユーザー名
- When `POST /register`
- Then 重複エラー（新規行なし）

- Given 正しい資格情報
- When `POST /login`
- Then ChallengeKey を返し、PlayerSessionKey は発行しない

- Given 誤った資格情報
- When `POST /login`
- Then チャレンジなしの失敗。3 回連続失敗後 15 分ロックアウト

- Given 有効 PlayerSessionKey
- When `POST /logout`
- Then 対応セッションが revoke される

## Adjudicator decisions（2026-08-20）

1. Spring Boot **3.4.x** / Java 17（ピン: **3.4.13** — 3.4 最終 OSS パッチ。OSS EOL 済のため
   後続で 3.5/4.x 移行を検討）
2. MyBatis Spring Boot Starter **3.0.x**
3. Green: 当面 **既存 JDBC を `@Bean` 配線**、MyBatis は同スライスで段階導入
4. エラー JSON: `{ "reason": "..." }`（`lockedUntil` は login lockout のみ）

## Phase 1 Red（2026-08-20）

- Tests: `seed-auth/backend/src/test/kotlin/com/seed/auth/adapter/http/PlayerAuthHttpAdapterTest.kt`
- Covered: register 201/409, login challenge/401/423+lockedUntil, logout 200
- Expected Red: compile failure（`PlayerAuthController` 未実装、Spring 依存未配線）
- Out of Red: controller / Spring Boot app / MyBatis / DataSource（Green）

## Phase 2 Green（2026-08-20）

- `PlayerAuthController` — `/register` `/login` `/logout`（UseCase 委譲のみ）
- Spring Boot **3.4.13** + `spring-boot-starter-web` + MockMvc tests
- MyBatis starter **3.0.5** on classpath; `MybatisAutoConfiguration` excluded
  （JDBC `@Bean` が当面のポート実装 — decision 3）
- `AuthPersistenceConfig` — `SEED_IDENTITY_DB_URL` があるとき JDBC adapters 配線
- Verification: `./gradlew test` BUILD SUCCESSFUL（HTTP 6 + UseCase 6；Postgres
  adapter は URL 未設定時 skip）

## Phase 3 Refactor（2026-08-20）

- HTTP JSON ヘルパー（`acceptedBody` / `reasonBody`）と DTO ファイル分離
- 挙動・アサーションは変更なし

## Completion（2026-08-20）

- Merged via PR #18 (`fe04ec8`).
- Remaining follow-ups: **LISS-0156**（MyBatis mapper + Boot upgrade）、
  LISS-0149 SPA、パスワードリセット API。

## English

Expose seed_auth UseCase via Spring Boot HTTP (`/register`, `/login`,
`/logout`) and introduce MyBatis wiring per ADR 0019/0023.

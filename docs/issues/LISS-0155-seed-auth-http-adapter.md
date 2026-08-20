# LISS-0155: `seed_auth` Spring Boot HTTP Adapter

- Status: open
- Phase: design
- Related branch: （未作成 — Phase 1 承認後 `feature/liss-0155-seed-auth-http-adapter`）
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
- MockMvc / WebTestClient による契約テスト（DB 未設定時の方針は Phase 1 で固定）

## スコープ外

- LISS-0149 React SPA
- ロックアウトの Postgres 永続化（プロセス内メモリ維持）
- `seed_server` Keep-Alive / challenge claim（LISS-0147）
- パスワードリセット API（将来）

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

## Remaining decisions（Adjudicator）

1. Spring Boot バージョン（提案: **3.4.x** + Java 17）
2. MyBatis Spring Boot Starter バージョン（提案: **3.0.x**）
3. 既存 JDBC Adapter（LISS-0151）を Green で MyBatis 置換するか、
   当面 JDBC を Spring `@Bean` 配線し MyBatis を並行導入するか
4. JSON エラーボディ形状（`reason` 文字列中心でよいか）

## English

Expose seed_auth UseCase via Spring Boot HTTP (`/register`, `/login`,
`/logout`) and introduce MyBatis wiring per ADR 0019/0023.

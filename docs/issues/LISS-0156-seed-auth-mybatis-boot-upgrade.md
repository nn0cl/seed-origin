# LISS-0156: `seed_auth` MyBatis Mapper 置換と Spring Boot アップグレード

- Status: open
- Phase: design
- Related branch: （未作成 — 決定・Phase 1 承認後）
- Priority: medium
- Depends on: LISS-0155（HTTP Adapter done — PR #18）
- Parent: LISS-0146 / LISS-0155
- Related ADR: `docs/architecture/adr/0019-admin-backend-language-kotlin-spring-boot.md`,
  `docs/architecture/adr/0023-player-auth-session-flow-details.md`
- Blocks: なし（0149 は HTTP 契約があれば進行可）

## 目的

LISS-0155 で残した二点を解消する。

1. ポート実装を JDBC 直書きから **MyBatis mapper** へ置換（ADR 0019/0023）
2. OSS EOL の Spring Boot **3.4.13** から **サポート中の Boot 系**へアップグレード

## スコープ

- `UserStore` / `PlayerChallengeStore` / `PlayerSessionStore` / `PasswordCodec`
  の永続化を MyBatis `@Mapper`（または XML）へ移行
- `AuthPersistenceConfig` の JDBC `fromEnvironment()` 直配線を DataSource +
  SqlSession ベースへ置換
- `MybatisAutoConfiguration` exclude の解除
- Spring Boot バージョンアップ（下記 Remaining decisions）
- 既存契約テストの維持:
  - `PlayerAuthHttpAdapterTest`（MockMvc）
  - `PlayerAuthenticationServiceTest`（UseCase）
  - `PostgresAuthAdapterTest`（DB 統合；URL 未設定時 skip）

## スコープ外

- LISS-0149 SPA
- ロックアウトの Postgres 永続化
- パスワードリセット API
- `seed_admin` Kotlin 移行との同時実施（別 Issue）

## 受入条件

- Given `SEED_IDENTITY_DB_URL` が設定されている
- When MyBatis 経由で user / challenge / session / pgcrypto を操作する
- Then LISS-0151 と同等の永続化・照合結果になる

- Given Spring Boot をアップグレードしたビルド
- When `./gradlew test` を実行する
- Then HTTP / UseCase /（DB あり時）Postgres adapter テストがすべて通る

- Given 本番起動経路
- When UseCase ポートが解決される
- Then JDBC 直書き Adapter クラスに依存しない（または thin wrapper のみ）

## Remaining decisions（Adjudicator）

1. **Spring Boot ターゲット**（2026-08 時点で 3.4/3.5 は OSS EOL）
   - 提案 A: **4.1.x**（現行 OSS サポート、推奨）
   - 提案 B: **4.0.x**（サポート短い）
   - 非推奨: 3.5.16（既に OSS EOL）
2. **MyBatis Spring Boot Starter**
   - Boot 4 なら提案: **mybatis-spring-boot-starter 4.x**
   - Boot 3 に留める場合のみ 3.0.x 継続
3. **移行単位**
   - 提案: 単一 Issue で Boot アップグレード + MyBatis 置換をまとめる
   - 代替: Boot だけ先、MyBatis は次 Issue

## English

Replace LISS-0151 JDBC port adapters with MyBatis mappers and upgrade
seed_auth off EOL Spring Boot 3.4 onto a supported Boot line.

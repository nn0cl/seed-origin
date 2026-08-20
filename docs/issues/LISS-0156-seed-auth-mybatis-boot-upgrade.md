# LISS-0156: `seed_auth` MyBatis Mapper 置換と Spring Boot アップグレード

- Status: done
- Phase: done
- Related branch: merged via PR #21 (`feature/liss-0156-mybatis-boot-upgrade`)
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
- Spring Boot **4.1.0** + MyBatis starter **4.1.0**
- 既存契約テストの維持:
  - `PlayerAuthHttpAdapterTest`（MockMvc）
  - `PlayerAuthenticationServiceTest`（UseCase）
  - MyBatis 統合テスト（旧 JDBC テストは退役）

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
- Then HTTP / UseCase /（DB あり時）MyBatis adapter テストがすべて通る

- Given 本番起動経路
- When UseCase ポートが解決される
- Then JDBC 直書き Adapter クラスに依存しない

## Adjudicator decisions（2026-08-20）

- **方針**: 各種ミドルウェアは **OSS サポート範囲内の最新** を採用する。
- Spring Boot: **4.1.0**（4.1 系の現行最新・OSS サポート中）
- MyBatis Spring Boot Starter: **4.1.0**（Boot 4.1 対応の現行最新）
- 範囲: **単一 Issue**（Boot アップグレード + MyBatis mapper 置換）
- その他（PostgreSQL JDBC・Kotlin プラグイン等）: Boot BOM / サポート行列に従い
  サポート内最新へ追随

## Phase 1 Red（2026-08-20）

- Tests: `seed-auth/backend/src/test/kotlin/com/seed/auth/adapter/mybatis/MyBatisAuthAdapterTest.kt`
- Covered: user save/find + pgcrypto; challenge issue; session revoke（0151 同等）
- Expected Red: compile failure（`MyBatisUserStore` 等未実装）

## Phase 2 Green（2026-08-20）

- Spring Boot **4.1.0** + Gradle **8.14.3** + Kotlin **2.1.21**
- MyBatis starter **4.1.0**; mappers: User/Password/Challenge/Session
- Stores: `MyBatisUserStore` / `MyBatisPasswordCodec` / challenge / session
- `AuthPersistenceConfig` が MyBatis を配線
- Boot 4: `spring-boot-starter-webmvc` + `webmvc-test`

## Phase 3 Refactor（2026-08-20）

- LISS-0151 JDBC Adapter 実装と `PostgresAuthAdapterTest` を削除（MyBatis が正）
- libpq URL 解析を `MyBatisSupport` に集約

## Completion（2026-08-20）

- Merged via PR #21 (`52b3507`).
- Remaining follow-ups: LISS-0149 SPA、パスワードリセット API、`seed_admin` Kotlin。

## English

Replace LISS-0151 JDBC port adapters with MyBatis mappers and upgrade
seed_auth off EOL Spring Boot 3.4 onto a supported Boot line.

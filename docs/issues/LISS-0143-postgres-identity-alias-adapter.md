# LISS-0143: PostgreSQL IdentityAliasStoreアダプタ

- Status: in_progress
- Phase: phase-2-green
- Priority: high
- Depends on: LISS-0130
- Related ADR: `docs/architecture/adr/0016-identity-alias-persistence-and-review.md`

## 目的

ADR 0016で決定したPostgreSQLを実バックエンドとする`IdentityAliasStore`アダプタを追加する。
`InMemoryIdentityAliasStore`はテスト・既定Baselineとして維持し、置き換えない。

## 受入条件

- `session::IdentityAliasStore`を実装する新アダプタ`PostgresIdentityAliasStore`を追加する。
- SQL、接続、トランザクション管理はアダプタ内に閉じ、`SessionRegistry`やドメイン層に
  漏らさない。
- 接続文字列は環境変数から取得し、ソースへ資格情報を埋め込まない。
- パラメータ化クエリのみを使用し、SQLインジェクションを避ける。
- ローカル開発・検証用のPostgreSQLはDocker Composeで構築する。
- libpqxxが見つからない環境でも既存のインメモリBaselineのビルドを壊さない
  （CMakeでオプション化する）。

## Dependency Adoption Note（libpqxx）

- **Security posture**: Homebrew配布の`libpqxx`最新安定版を導入する。既知の重大な
  CVEは本セッション時点で確認していない（要継続確認）。
- **Version-specific examples**: libpqxx 7系はC++17以降が前提、本プロジェクトは
  C++20（ADR 0015）のため互換。
- **Troubleshooting depth**: libpqxxは公式ドキュメント・サンプルが豊富で
  トラブルシュート情報は十分。
- **Minimal real-file test**: Docker Compose起動のPostgreSQLに対しparameterized
  insert/select/updateを行う統合テストを追加する（ビルドのみ確認、実行はしない）。
- **POC feasibility**: 本Issueの実装そのものをPOCとして扱う。
- **Boundary fit**: `src/PostgresIdentityAliasStore.cpp`と対応ヘッダのみが
  libpqxxをincludeする。他のsrc/includeファイルは`IdentityAliasStore`ポート越しに
  しか触れない。

## Implementation slice

- `db/docker-compose.yml`: ローカル開発用PostgreSQL 16コンテナ。
- `db/migrations/0001_identity_aliases.sql`: `identity_aliases`テーブルのDDL。
- `include/seed/PostgresIdentityAliasStore.h` / `src/PostgresIdentityAliasStore.cpp`:
  `IdentityAliasStore`実装。接続文字列は`SEED_IDENTITY_DB_URL`環境変数から取得する。
- `CMakeLists.txt`: `pkg_check_modules`でlibpqxxを検出した場合のみアダプタと
  対応する統合テストをビルド対象に加える（`SEED_HAVE_LIBPQXX`）。

## Remaining decisions

- 保存期間purgeジョブ、admin認証、export管理者ページはADR 0016の別ambiguityとして
  未着手。
- マイグレーションツール（今回は素のSQLファイルを手動適用する前提）の自動化方針は
  未決定。

## Verification

- Docker Composeで`identity-postgres`コンテナを起動し、
  `db/migrations/0001_identity_aliases.sql`が自動適用され`identity_aliases`
  テーブルが期待通りのスキーマで作成されることを`psql \d`で確認した。
- `PKG_CONFIG_PATH`にHomebrewのkeg-only`libpq`/`libpqxx`を追加した上で、
  警告有効C++20ビルドが成功することを確認した（`seed_postgres`、
  `seed_postgres_tests`を含む）。
- `libpqxx`未検出環境（`PKG_CONFIG_PATH`を設定しない別ビルドディレクトリ）でも
  既存の`seed_core`・`seed_cli`・`seed_tests`のみが問題なくビルドされることを
  確認した。
- Adjudicator方針によりテスト実行（`ctest`・`seed_postgres_tests`の起動）は
  保留した。Docker Composeコンテナは検証後に停止・削除済み（データボリュームは
  保持）。

## English

Add a PostgreSQL-backed `IdentityAliasStore` adapter per ADR 0016, keeping the
in-memory adapter as the default/test baseline. SQL and connection handling
stay inside the adapter; the build stays green even without libpqxx installed.

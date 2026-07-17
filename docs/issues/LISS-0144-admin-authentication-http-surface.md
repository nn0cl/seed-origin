# LISS-0144: 管理者認証・エクスポート・衝突解決HTTP API

- Status: in_progress
- Phase: phase-2-green
- Priority: high
- Depends on: LISS-0130, LISS-0143
- Related ADR: `docs/architecture/adr/0017-admin-authentication-and-http-surface.md`

## 目的

ADR 0016が要求する「管理者ログイン必須のエクスポート」「キュー型衝突解決UI」を、
ADR 0017で決定した認証方式（`admin_users`テーブル + pgcrypto bcrypt + Bearer
トークン）で実現するHTTP APIを追加する。ブラウザUIは対象外（ADR 0017 決定5）。

## 受入条件

- `admin_users`テーブルにない資格情報ではログインできない。
- ログイン成功時のみBearerトークンを発行し、以後のエンドポイントは
  `Authorization: Bearer <token>`必須。
- トークンなし・不正・期限切れの場合は401相当を返す。
- エクスポート・衝突解決キュー取得・レビュー確定はすべて認証必須。
- パスワードはpgcryptoのbcryptハッシュのみで保持し、平文をログへ出力しない。
- `seed_admin`はlibpqxxとcpp-httplibの両方が検出された場合のみビルド対象になる
  （既存の`SEED_HAVE_LIBPQXX`パターンを踏襲）。

## Implementation slice

- `db/migrations/0002_admin_users.sql`: `pgcrypto`拡張の有効化と`admin_users`
  テーブル。
- `include/seed/AdminSessionStore.h` / `src/AdminSessionStore.cpp`
  （`seed_core`、外部依存なし）: `/dev/urandom`由来のBearerトークン発行・検証・
  失効。テスト用にトークン生成関数を注入可能にした。
- `include/seed/AdminAuthStore.h` / `src/AdminAuthStore.cpp`（`seed_postgres`、
  libpqxx依存）: `admin_users`に対する`crypt()`検証のみを行う。
- `src/AdminMain.cpp`（`seed_admin`実行ファイル、libpqxx + cpp-httplib依存）:
  `POST /login`、`POST /logout`、`GET /aliases/export`、
  `GET /aliases/review-queue`、`POST /aliases/review`。
- `CMakeLists.txt`: `find_package(httplib CONFIG QUIET)`を追加し、
  `LIBPQXX_FOUND AND httplib_FOUND`のときのみ`seed_admin`をビルドする。

## Remaining decisions（2026-07-18更新）

- 解決済み: `/login`のレート制限は`AdminLoginLockout`で実装した
  （username単位、3回連続失敗でロック）。ロック解除までの待機時間
  （15分）はAdjudicatorから明示指定がなかったための仮値であり、
  必要であれば別途確定させる。
- 解決済み: セッションTTLは1時間固定（アイドルタイムアウトなし）に決定・
  反映した。複数インスタンス間のセッション共有は単一プロセス運用が前提の
  ため現時点では対象外。
- 解決済み: TLS終端はローカル運用のみでよいと確定（`127.0.0.1` bindの
  ままで変更不要）。
- ブラウザUIは`docs/issues/LISS-0145-admin-ui-react-spa.md`として
  Issue化し、React SPA + SSEの方針は決定したが、npmツールチェーン選定・
  配置場所・SSEペイロード契約・Bearer認証とSSEの統合方式は
  設計未着手（Phase 0 design intake）。
- `seed_admin`にSIGTERM/SIGINTの明示的なgraceful shutdownハンドラがない
  （`seed_server`と異なり、cpp-httplibの`listen()`はOSのデフォルト終了に
  委ねている）。

## Verification

- 警告有効C++20ビルドを、libpqxx・cpp-httplib両方検出／`PKG_CONFIG_PATH`・
  `CMAKE_PREFIX_PATH`未設定（両方未検出）の2構成で実行し、後者でも既存
  ターゲット（`seed_core`・`seed_cli`・`seed_server`・`seed_tests`）のみが
  問題なくビルドされることを確認した。
- Docker ComposeのPostgreSQLへ`db/migrations/0002_admin_users.sql`を適用し、
  `crypt()`でブートストラップ管理者アカウントを1件作成した上で、`curl`による
  手動接続テストを行った。
  - 誤ったパスワードでの`POST /login` → `401`。
  - 正しい資格情報での`POST /login` → トークン取得。
  - トークンなしでの`GET /aliases/export` → `401`。
  - トークンありでの`GET /aliases/export` → `200`、既存alias 1件を確認。
  - `GET /aliases/review-queue` → 未レビューの同aliasを確認。
  - `POST /aliases/review`（confirmed、confidence=0.9） → `204`。
  - 確定後の`review-queue` → 空配列、`export` → `reviewStatus=confirmed`・
    `confidence=0.9`に更新済みであることを確認。
  - `POST /logout` → `204`、失効後の同トークンでの`export` → `401`。
- `AdminLoginLockout`追加後、同一usernameで3回連続誤ったパスワードを
  送信 → 4回目は正しいパスワードでも`423`（アカウントロック）を返すことを
  `curl`で確認した。
- Adjudicator方針によりCTest（`ctest`）は保留した。検証後、`seed_admin`
  プロセスとDocker Composeコンテナは停止・削除済み（データボリュームは
  保持）。

## English

Adds a minimal admin authentication and JSON/form HTTP API (login, export,
review queue, review submission) per ADR 0017, independent from the
unauthenticated player identity model. No browser UI; curl-testable only.

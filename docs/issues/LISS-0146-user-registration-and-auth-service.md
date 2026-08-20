# LISS-0146: `users`テーブルと`seed_auth`（登録・ネイティブログイン・Postgresセッション）

- Status: done
- Phase: usecase-slice-complete
- Related branch: merged via PR #4 (`feature/liss-0147-challenge-session-login`)
- Priority: high
- Depends on: なし（ADR 0018が起点）
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

登録制プレイヤー認証の基盤となる`users`テーブルと、資格情報検証・
セッション発行を担う`seed_auth`実行ファイルを追加する。

## 受入条件（設計確定）

### データ所有とライフサイクル

- `users`は`seed_auth`が所有し、`id`と一意な`username`をプレイヤーの
  永続的な識別子とする。パスワードは`pgcrypto`のハッシュとして保存し、
  平文パスワードを保存しない。
- `player_challenges`は`seed_auth`が発行する`ChallengeKey`を保存する。
  チャレンジは`user_id`、発行時刻、期限（2分）、claim時刻を持ち、
  一度claimされたチャレンジは再利用できない。
- `player_sessions`は`PlayerSessionKey`を保存する。正規セッションは
  `user_id`、発行時刻、期限（30分）、失効時刻を持つ。期限はクライアント
  起点のKeep-Aliveによって延長できる。
- `player_challenges`と`player_sessions`は複数の`seed_server`インスタンス
  から参照できるよう、PostgreSQLを唯一の共有状態とする。in-memoryの
  セッションレジストリを認証の正本にしない。

### API受入シナリオ

以下はPhase 1 Redでテスト化する契約であり、HTTPフレームワークやDB実装を
固定するものではない。

#### 登録

- Given: 未登録のユーザー名と有効なパスワードが入力される
- When: `POST /register`を呼び出す
- Then: `users`に一意なユーザーが作成され、パスワードハッシュだけが保存される

- Given: 既存のユーザー名が入力される
- When: `POST /register`を呼び出す
- Then: 新しいユーザーは作成されず、重複エラーを返す

#### ネイティブクライアント用チャレンジ発行

- Given: 登録済みユーザーの正しい資格情報がネイティブクライアントから送られる
- When: `POST /login`を呼び出す
- Then: `player_challenges`に単回利用の`ChallengeKey`が作成され、クライアントへ返される
- And: `seed_auth`は`PlayerSessionKey`を発行しない

- Given: 誤った資格情報または存在しないユーザーが送られる
- When: `POST /login`を呼び出す
- Then: チャレンジキーを発行せず、認証失敗を返す
- And: 同一ユーザーの3回連続した認証失敗後は15分間ログインを拒否する

#### セッション失効

- Given: 有効な`PlayerSessionKey`が存在する
- When: `POST /logout`を呼び出す
- Then: 対応する`player_sessions`が失効状態になり、以後の検証対象外になる

### セキュリティ境界

- `seed_auth`はパスワードの検証と`ChallengeKey`の発行を担当する。
- `seed_auth`は3回連続したログイン失敗に対して15分間のロックアウトを適用する。
- `seed_server`は`ChallengeKey`をclaimし、`PlayerSessionKey`を発行する。
- `seed_server`にはパスワード、パスワードハッシュ、アカウント管理用の
  資格情報を渡さない。
- SPAはゲームプレイ用ログイン経路に参加せず、ゲーム用キーを保持・転送しない。
- `seed_auth`は`seed_admin`・`seed_server`と別プロセス・別実行ファイルとする。

## ディレクトリ構造・Dockerネットワーク（2026-07-18確定）

```
seed-auth/
├── docker-compose.yml
├── backend/            # Kotlin + Spring Boot、LISS-0146のスコープ
│   └── ...
└── frontend/           # LISS-0149のスコープ
    └── src/...
```

`seed-admin`と同じ外部共有Dockerネットワーク（`seed-network`、
LISS-0145参照）に参加する。`db/docker-compose.yml`・
`seed-admin/docker-compose.yml`・`seed-auth/docker-compose.yml`が
共通の`seed-network`を参照する構成。

## 資格情報の流通境界（2026-07-18、Adjudicator確認済み）

パスワードは、用途ごとに次の境界で扱う。

- ゲームプレイ用ログイン: ネイティブクライアントと`seed_auth`の間だけで
  扱う。ブラウザSPAはこの認証経路に参加しない。
- アカウント管理: `seed-auth/frontend`（LISS-0149）と`seed_auth`の間で、
  新規登録、アカウント情報変更、パスワードリセットに必要な範囲だけ扱う。

`seed_server`とクライアントの間、`seed_server`と`seed_auth`の間は
**一回限りのチャレンジキーまたは正規セッションキーのみ**が流れ、パスワード・
ハッシュは一切渡さない（LISS-0147参照）。

## 実装言語（2026-07-18確定、ADR 0023決定1）

`seed_auth`はKotlin + Spring Boot（`seed_admin`のADR 0019と同一アーキテクチャ:
Gradle Kotlin DSL、MyBatis、DDD + Clean Architecture層構造）とする。C++/
libpqxx+cpp-httplibパターンの記述は撤回済み（今回で確定に格上げ）。

## Phase 1 設計着手（2026-07-22、ADR 0023承認済み）

- ネイティブクライアントが`seed_auth`から2分TTLの使い捨て
  `ChallengeKey`を取得する。
- `seed_server`がチャレンジキーを検証し、30分TTLの`PlayerSessionKey`を発行する。
- `PlayerSessionKey`はクライアント起点のKeep-Aliveで更新・延長する。

Phase 1では、登録、重複ユーザー名、パスワード検証、チャレンジキーの単回利用・
期限、ログインロックアウト、セッション無効化をRedテストとして設計する。
正規セッションの発行・検証・Keep-AliveはLISS-0147のRedテストで扱う。DB、HTTP、
時刻、外部サービスはポート越しに差し替える。

## Phase 1 Red artifact

- Test: `seed-auth/backend/src/test/kotlin/com/seed/auth/application/PlayerAuthenticationServiceTest.kt`
- Phase 1 review: approved. Phase 2 state: implementation added; deterministic
  Gradle verification is pending because Gradle is not installed in the current
  environment.
- Covered scenarios: 登録、重複ユーザー名、チャレンジ発行、無効資格情報、2分TTL、
  3回失敗後15分ロックアウト、ログアウト失効

## Phase 2 Green slice

- `seed-auth/backend/build.gradle.kts`: Kotlin/JVMの単体テスト実行基盤のみ。
- `seed-auth/backend/src/main/kotlin/com/seed/auth/domain/PlayerKeys.kt`:
  `ChallengeKey`と`PlayerSessionKey`。
- `seed-auth/backend/src/main/kotlin/com/seed/auth/application/port/Ports.kt`:
  `UserStore`、`PlayerChallengeStore`、`PlayerSessionStore`、`PasswordCodec`、
  `Clock`。
- `seed-auth/backend/src/main/kotlin/com/seed/auth/application/PlayerAuthenticationService.kt`:
  登録、チャレンジ発行、ロックアウト、ログアウト失効のUseCase。
- `seed-auth/docker-compose.yml`: 外部共有 `seed-network` への参加予約
  （Spring プロセスは HTTP Adapter スライスで追加）。

このGreen sliceにはSpring HTTP、MyBatis、PostgreSQL Adapter、実際の
`pgcrypto`接続、正規セッションの発行・Keep-Aliveは含めない。後者はLISS-0147
および対応するAdapter実装のスコープとする。

## Phase 2 verification（2026-08-20）

- Gradle Wrapper `8.11.1` を追加。
- Foojay toolchain resolver により JDK 17 を自動取得（ホストに JDK 17 が
  無くても `jvmToolchain(17)` が解決される）。
- 検証コマンド（Corretto 23 ホスト上）:

```text
cd seed-auth/backend
JAVA_HOME=<jdk-23-or-newer> ./gradlew test --no-daemon
# BUILD SUCCESSFUL — PlayerAuthenticationServiceTest 6 cases
```

- Dependency adoption: Kotlin `2.0.21` + Gradle Wrapper 実行確認済み。
  脆弱性監査の継続は Adapter 導入時に再確認する。

## UseCase slice done（2026-08-20）

- Merged via PR #4. Application UseCase + Gradle verification are on main.
- Remaining outside UseCase completion: Spring HTTP, MyBatis, PostgreSQL
  Adapter, real `pgcrypto`.
- Follow-up Issue for Postgres Adapter: **LISS-0151**（done — PR #16）.
- Follow-up Issue for Spring HTTP Adapter: **LISS-0155**（done — PR #18）.
- Remaining after 0155: MyBatis mapper 置換、Spring Boot 3.5/4.x、LISS-0149 SPA.

## 設計契約（Phase 1 Redの入力）

### VO／DTO候補

- `UserId`: 登録済みユーザーを識別する永続ID。
- `Username`: 一意性、許可文字、長さ制限を検証する値オブジェクト。
- `ChallengeKey`: 2分・単回利用の認証チャレンジ。`seed_server`へのLogin
  payloadにだけ使用する。
- `PlayerSessionKey`: 30分TTLの正規セッション。通常Command、Keep-Alive、
  再接続で使用する。
- `RegisterUserCommand`、`NativeLoginCommand`、`IssueChallengeResult`、
  `LogoutCommand`: HTTP DTOとユースケース間の境界契約候補。

### Ports／Adapters

- `UserStore`: ユーザー作成、ユーザー名検索、パスワードハッシュ取得を抽象化する。
- `PlayerChallengeStore`: チャレンジ作成、期限検証に必要な状態を保持する。
- `PlayerSessionStore`: 正規セッション作成、期限延長、失効、検証を抽象化する。
- `PasswordCodec`: ハッシュ生成・`pgcrypto`検証をユースケースから分離する。
- `Clock`: チャレンジ期限、セッション期限、Keep-Alive延長を決定論的に検証する。
- PostgreSQL、Spring HTTP、MyBatisはAdapter層に限定し、Domain／UseCaseへ直接
  露出させない。

### テスト配置

LISS-0146はKotlin + Spring Bootサービスのため、Phase 1 Redテストは
`seed-auth/backend/src/test/kotlin/`に配置し、Gradleのテストタスクで実行する。
ルートのC++ `tests/`および`seed_tests`には追加しない（ADR 0024）。

### スコープ外

- `seed_server`側のチャレンジclaim実装とLogin Command改修（LISS-0147）。
- アカウント管理SPAの画面・APIクライアント（LISS-0149）。
- プレイヤー進行・アイテム永続化（LISS-0148）。
- ゲームクライアントのログインUIとKeep-Alive送信実装。

## 設計課題（Phase 1後の設計候補）

- 登録時のユーザー名重複・禁止文字・長さ制限のポリシー。
- チャレンジキーの保存形態と、ロックアウト状態の保持単位（ユーザー名、
  IP、またはその組み合わせ）。TTL値とロックアウト回数・期間は確定済み。
- `seed_auth`と`seed_admin`のセッションストア実装を共通化するか
  （`AdminSessionStore`をPostgres版に拡張して両者から使うか、独立させるか）。

## Dependency adoption note

- Kotlin Gradle plugin: `2.0.21`、test helper: Kotlin `test`。
- 用途: `seed_auth`アプリケーション層のコンパイルとJUnit Platformテスト。
- 境界: Kotlin/JVMのビルド・テスト専用。Domain／UseCaseへSpringやPostgreSQLの
  型を持ち込まない。
- 状態: manifestと実ソースは追加済みだが、Gradleが現環境にないため、依存性の
  脆弱性監査、実行確認、バージョン互換性確認は未実施。Phase 2の完了条件として
  Gradle導入後に確認する。

## Remaining decisions

- 実装言語・二段階キー・TTL・ロックアウト方針は確定済み。Phase 1 Redのテストレビュー後に
  Phase 2 Greenへ進む。

## English

Phase 1 Red design for the `users` table, Postgres-backed challenge/session
tables, and a new `seed_auth` executable (registration, native-client login,
logout) per ADR 0018 and accepted ADR 0023. No implementation has started;
remaining design questions (lockout policy, port split, and session-store code
sharing with seed_admin) are listed above.

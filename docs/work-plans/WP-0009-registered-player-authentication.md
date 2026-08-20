# WP-0009: 登録制プレイヤー認証への移行

## 目的

ADR 0018に基づき、匿名ログイン＋ID名寄せ（LISS-0123／LISS-0130／ADR 0016）を
登録制ユーザー認証（`users`テーブル、Postgres裏付けの`player_challenges`と
`player_sessions`、
`seed_auth`サービス）へ置き換える。本WorkPlanはその移行の依存グラフを
canonicalに定義する。

## 前提（ADR 0023 Open Questions解決に基づく最新版）

- 管理者UI（LISS-0145）と同じくReact SPAをフロントエンドに使う。
- ワールドサーバー（`seed_server`）は複数インスタンス展開を想定するため、
  セッションストアはin-memoryではなくPostgresに保存する。
- `seed_server`と`seed_admin`は別インスタンス。新設する`seed_auth`も
  両者と別インスタンスとする。
- パスワード検証は`pgcrypto`（ADR 0017で確立したパターン）を再利用し、
  新規C++暗号ライブラリは追加しない。
- `seed_auth`の実装言語はKotlin + Spring Boot（`seed_admin`と同一
  アーキテクチャ、ADR 0023決定1）。
- チャレンジキーはネイティブクライアントが`seed_auth`から取得し、TTLは
  2分の単回利用とする。
- `seed_server`が検証後に発行する正規セッションキーのTTLは30分とし、
  クライアント起点のKeep-Aliveで更新・延長する。
- ゲームプレイログインはネイティブクライアント内で完結し、LISS-0149の
  SPAはアカウント登録・管理・パスワードリセットに限定する。
- 詳細フローは`docs/specs/player-authentication-flow-v1.md`を参照。

## Canonical実行順

| 順序 | Issue | 内容 | 状態 |
| --- | --- | --- | --- |
| 0 | ADR 0018 | 登録制認証アーキテクチャ決定 | accepted |
| 1 | LISS-0146 | `users`・`player_challenges`・`player_sessions`・`seed_auth` | done（UseCase）；Adapter は LISS-0151→HTTP 後続 |
| 1.1 | LISS-0151 | `seed_auth` PostgreSQL / pgcrypto Adapter | in_progress / phase-3-refactor（JDBC；MyBatis→HTTP；branch `feature/liss-0151-seed-auth-postgres-adapter`） |
| 2 | LISS-0147 | チャレンジclaim・正規セッション発行・検証・更新 | done（PR #7–#12; last: #12 remove anonymous login） |
| 3 | LISS-0148 | クラシックMMORPG型プレイヤープログレッション永続化 | phase-1-red（depends: 0146） |
| 4 | LISS-0149 | 登録・アカウント管理用React SPA（ゲームログインなし） | phase-1-red（depends: 0146） |
| 5 | LISS-0150 | LISS-0123／LISS-0130／ADR 0016の廃止処理 | done（PR #14 docs supersession；コード削除は後続） |

## 廃止対象の扱い

- LISS-0123（匿名ログイン）・LISS-0130（ID名寄せ永続化）・ADR
  0016（名寄せの保存期間・衝突解決UI）は削除せず、`superseded`として
  ステータス更新する（`docs/collaboration/local-issue-planning.md`の方針に
  従い、仕様の出発点・依存関係は保持する）。
- `docs/architecture/README.md`のADR一覧・Non-Decision一覧、
  `docs/work-plans/WP-0007-remaining-issues.md`のcanonical表からも
  該当行を外す。

## 運用ゲート

各Issueは、受入条件・テスト資料・CodeQL確認・mainコミット・mainプッシュの
順で完了とする。今回のセッションではAdjudicator方針によりCTest実行を
保留しているため、警告有効ビルドと手動接続テストを最低限の検証とする。

## English

Canonical execution plan for replacing anonymous login + alias
reconciliation (LISS-0123/LISS-0130/ADR 0016) with registered-user
authentication (ADR 0018): `users`, `player_challenges`, and
`player_sessions` tables plus a new `seed_auth` service, followed by the
world server's challenge/session migration, player data persistence, an
account-management React SPA without game-play login, and formal deprecation
of the superseded pieces.

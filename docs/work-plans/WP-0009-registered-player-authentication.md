# WP-0009: 登録制プレイヤー認証への移行

## 目的

ADR 0018に基づき、匿名ログイン＋ID名寄せ（LISS-0123／LISS-0130／ADR 0016）を
登録制ユーザー認証（`users`テーブル、Postgres裏付けの`player_sessions`、
`seed_auth`サービス）へ置き換える。本WorkPlanはその移行の依存グラフを
canonicalに定義する。

## 前提（2026-07-18時点で確定済み）

- 管理者UI（LISS-0145）と同じくReact SPAをフロントエンドに使う。
- ワールドサーバー（`seed_server`）は複数インスタンス展開を想定するため、
  セッションストアはin-memoryではなくPostgresに保存する。
- `seed_server`と`seed_admin`は別インスタンス。新設する`seed_auth`も
  両者と別インスタンスとする。
- パスワード検証は`pgcrypto`（ADR 0017で確立したパターン）を再利用し、
  新規C++暗号ライブラリは追加しない。

## Canonical実行順

| 順序 | Issue | 内容 | 状態 |
| --- | --- | --- | --- |
| 0 | ADR 0018 | 登録制認証アーキテクチャ決定 | accepted |
| 1 | LISS-0146 | `users`テーブル・`seed_auth`（登録・ログイン・Postgresセッション） | proposed |
| 2 | LISS-0147 | ワールドサーバーのセッショントークン検証ログインへの置換 | proposed（depends: 0146） |
| 3 | LISS-0148 | プレイヤー特性・アイテムのPostgres永続化スキーマ | proposed（depends: 0146） |
| 4 | LISS-0149 | 登録・ログインReact SPA | proposed（depends: 0146） |
| 5 | LISS-0150 | LISS-0123／LISS-0130／ADR 0016の廃止処理 | proposed（depends: 0147） |

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
authentication (ADR 0018): a `users` table, Postgres-backed
`player_sessions`, and a new `seed_auth` service, followed by the world
server's login-flow migration, player data persistence, a React
registration/login SPA, and formal deprecation of the superseded pieces.

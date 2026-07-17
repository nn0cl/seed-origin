# LISS-0145: 管理者UI（React + Vite SPA）

- Status: proposed
- Phase: phase-0-design-intake
- Priority: medium
- Depends on: LISS-0144（バックエンド実装は今後ADR 0019によりKotlin/Spring
  Bootへ移行するが、Bearerトークン認証契約自体は維持される）
- Related ADR: `docs/architecture/adr/0017-admin-authentication-and-http-surface.md`,
  `docs/architecture/adr/0019-admin-backend-language-kotlin-spring-boot.md`

## 目的

管理者バックエンド（ADR 0019によりKotlin/Spring Boot）のHTTP APIに、
React SPAのブラウザUIを追加する。管理者がログイン・エクスポート・
衝突解決キューの確認とレビュー確定を、ブラウザから行えるようにする。

## 解決済みの設計課題（2026-07-18更新）

1. **npm/ビルドツールチェーン**: **Vite**に決定（ADR 0019で確定）。
2. **リアルタイム更新方式**: SSEではなく**ポーリング**（既存のBearer
   トークンをそのまま`fetch`で使い、`GET /aliases/review-queue`等を
   一定間隔で再取得する）に決定。これによりSSEと`EventSource`の
   ヘッダ制約問題は解消済み。
3. **デプロイ**: Docker Compose（ADR 0019で確定、`db/docker-compose.yml`
   にKotlin/Spring Bootバックエンドと合わせて追加）。

## 残る設計課題（Must not guess）

1. **フロントエンドの配置場所**: `AGENTS.md`の`project-structure.md`は
   `client/`をLISS-0064（ゲームクライアント技術選定）向けに予約している。
   管理者UIをそこに同居させるか、`admin-ui/`のような別ディレクトリに
   分離するかを決める必要がある（ゲームクライアントとは技術的に無関係な
   別Non-Decisionのため、混在させない方が安全と考えられる）。
2. **ビルド成果物の配信**: Vite/ReactのビルドをKotlin/Spring Boot側から
   配信するのか、別途Nginx等で配信するのかを決める。
3. LISS-0149（プレイヤー登録・ログインSPA）と同一アプリにするか別アプリに
   するかは未決定（対象ユーザー・バックエンドが異なるため別アプリを
   推奨するが未決定）。

## 受入条件（ドラフト、Phase 0時点の仮）

- ログイン画面、エクスポート一覧表示、衝突解決キュー表示、
  Confirm/Rejectボタンを持つ最小SPA。
- ポーリングでキューの変化が画面に反映される。
- 既存のBearerトークン認証契約と矛盾しない。
- 新規npm依存はdependency-policy.mdのadoption checklistを満たす。

## Remaining decisions

- 上記の残る設計課題はすべて未決定。ADR 0019がAdjudicator承認された後、
  Phase 1（Red）着手前に確定させる。

## English

Design intake for a React + Vite admin SPA on top of the (soon
Kotlin/Spring Boot) admin backend. Build tooling (Vite) and update
mechanism (polling, not SSE) are now decided; directory placement and
static-asset serving remain open. No implementation has started.

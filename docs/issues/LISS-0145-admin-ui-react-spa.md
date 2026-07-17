# LISS-0145: 管理者UI（React + TypeScript SPA）

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

## ディレクトリ構造（2026-07-18確定）

```
seed-admin/
├── docker-compose.yml
├── backend/            # Kotlin + Spring Boot（ADR 0019）
│   └── src/main/kotlin/...
└── frontend/           # 本Issueのスコープ
    ├── package.json
    ├── vite.config.ts
    └── src/
        ├── main.tsx
        ├── app/         # ルーティング・レイアウト
        ├── pages/       # LoginPage, ExportPage, ReviewQueuePage
        ├── components/  # 再利用可能なUI部品（shadcn/ui生成物含む）
        ├── store/       # Zustandストア（認証トークン等）
        ├── api/         # TanStack Queryフック・APIクライアント
        └── types/       # バックエンドDTOに対応する型定義
```

`client/`（LISS-0064/0020のゲームクライアント用に予約済み）とは別の、
リポジトリ直下の独立ディレクトリとする。LISS-0149（プレイヤー登録・
ログインSPA）とは対象ユーザー・バックエンドが異なるため、`seed-auth/`
配下に別アプリとして分離する（同一アプリにはしない）。

## フロントエンド技術スタック（2026年時点のトレンド調査に基づき確定）

- **ビルドツール**: Vite
- **言語**: TypeScript
- **サーバー状態管理**: TanStack Query（`GET /aliases/review-queue`等の
  ポーリング取得・キャッシュ・再取得を担当）
- **クライアント状態管理（store）**: Zustand（認証トークン等のグローバル
  状態）。サーバー状態とクライアント状態を明確に分離するのが2026年時点の
  主流パターン
- **UIコンポーネント**: shadcn/ui + Tailwind CSS v4（2026年時点で
  デファクトスタンダード、Radix UIベースでアクセシビリティ対応）
- **フォーム**: React Hook Form + Zod（バリデーション）
- **ソースコード分割方針**: 1枚のゴッドファイルを作らず、上記ディレクトリ
  構造（`pages/`/`components/`/`store/`/`api/`/`types/`）に責務ごとに
  分割する

## Dockerネットワーク（2026-07-18確定）

`seed-admin`と`seed-auth`（LISS-0146/0149）を同じDockerネットワークに
所属させるため、外部共有ネットワークを1つ定義する。

```yaml
networks:
  seed-network:
    external: true
```

`db/docker-compose.yml`・`seed-admin/docker-compose.yml`・
`seed-auth/docker-compose.yml`の3つがこの`seed-network`を参照する
（事前に`docker network create seed-network`が必要）。

## 解決済みの設計課題（2026-07-18更新）

1. **npm/ビルドツールチェーン**: Vite（確定）。
2. **リアルタイム更新方式**: SSEではなくポーリング（既存のBearerトークンを
   そのまま`fetch`で使い、`GET /aliases/review-queue`等を一定間隔で
   再取得する）。
3. **デプロイ**: Docker Compose（上記ネットワーク構成で確定）。
4. **フロントエンドの配置場所**: `seed-admin/frontend/`（確定、上記
   ディレクトリ構造参照）。
5. **LISS-0149との関係**: 別アプリとする（確定）。

## 残る設計課題（Must not guess）

1. **ビルド成果物の配信**: Vite/ReactのビルドをKotlin/Spring Boot側から
   配信するのか、別途Nginx等で配信するのかを決める。

## 受入条件（ドラフト、Phase 0時点の仮）

- ログイン画面、エクスポート一覧表示、衝突解決キュー表示、
  Confirm/Rejectボタンを持つ最小SPA。
- ポーリングでキューの変化が画面に反映される。
- 既存のBearerトークン認証契約と矛盾しない。
- 新規npm依存（TanStack Query、Zustand、shadcn/ui、Tailwind CSS、React
  Hook Form、Zod）はdependency-policy.mdのadoption checklistを満たす。
- ソースコードは上記ディレクトリ構造に沿って分割され、単一の巨大ファイルに
  ならない。

## Remaining decisions

- ビルド成果物の配信方式のみ未決定。ADR 0019がAdjudicator承認された後、
  Phase 1（Red）着手前に確定させる。

## English

Design intake for a React + TypeScript + Vite admin SPA on top of the
(soon Kotlin/Spring Boot) admin backend, living in `seed-admin/frontend/`.
State management splits server state (TanStack Query) from client state
(Zustand), UI uses shadcn/ui + Tailwind CSS v4, forms use React Hook Form +
Zod. `seed-admin` and `seed-auth` share a Docker network. Only static-asset
serving remains undecided. No implementation has started.

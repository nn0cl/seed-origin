# LISS-0149: 登録・ログインReact SPA

- Status: proposed
- Phase: phase-0-design-intake
- Priority: medium
- Depends on: LISS-0146
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

`seed_auth`の`/register`・`/login`を利用する、プレイヤー向け登録・
ログインReact SPAを追加する。

## ディレクトリ構造（2026-07-18確定）

```
seed-auth/
├── docker-compose.yml
├── backend/             # LISS-0146のスコープ
└── frontend/            # 本Issueのスコープ
    ├── package.json
    ├── vite.config.ts
    └── src/
        ├── main.tsx
        ├── app/          # ルーティング・レイアウト
        ├── pages/        # RegisterPage, LoginPage
        ├── components/   # 再利用可能なUI部品（shadcn/ui生成物含む）
        ├── store/        # Zustandストア（セッションキー等）
        ├── api/          # TanStack Queryフック・APIクライアント
        └── types/        # バックエンドDTOに対応する型定義
```

管理者UI（`seed-admin/frontend/`、LISS-0145）とは**別アプリ**として
`seed-auth/frontend/`に分離する（確定。対象ユーザー・バックエンドが
異なるため）。`seed-admin`と同じ外部共有Dockerネットワーク
（`seed-network`）に参加する。

## フロントエンド技術スタック（LISS-0145と共通、2026-07-18確定）

- ビルドツール: Vite、言語: TypeScript
- サーバー状態管理: TanStack Query
- クライアント状態管理（store）: Zustand（セッションキー等）
- UIコンポーネント: shadcn/ui + Tailwind CSS v4
- フォーム: React Hook Form + Zod
- ソースコード分割方針: 1枚のゴッドファイルを作らず、責務ごとに
  `pages/`/`components/`/`store/`/`api/`/`types/`へ分割する

## 解決済みの設計課題（2026-07-18更新）

- npmツールチェーンは**Vite**に決定。
- 管理者UI（LISS-0145）とは**別アプリ**に決定（確定）。

## 設計課題

- ログイン成功後のセッションキー（LISS-0146/0147の「先勝ちローテーション」
  設計を参照）をゲームクライアント（ADR 0021、Unreal Engine）へどう
  引き継ぐか（ブラウザSPAとUnrealクライアントが別プロセス/別技術の
  場合の橋渡し）。
- パスワードそのものは`seed_auth`とこのSPAの間でのみやり取りし、
  `seed_server`には一切渡さない（Adjudicator確認済み、ADR 0018/
  LISS-0147と整合）。

## Remaining decisions

- ゲームクライアントへのセッションキー引き継ぎ方式が未決定。
  ゲームクライアント技術自体はADR 0021（Unreal Engine、本番）／
  ADR 0020（Godot、MVP）で決定済み。

## English

Design intake for a React registration/login SPA calling seed_auth. Shares
open build-tooling decisions with LISS-0145 (admin UI). How the session
token hands off to the (not-yet-decided) game client is an open question.

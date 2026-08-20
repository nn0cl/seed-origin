# LISS-0149: アカウント管理用React SPA

- Status: proposed
- Phase: phase-1-red
- Priority: medium
- Depends on: LISS-0146
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

`seed_auth`を利用する、プレイヤー向けのアカウント管理React SPAを追加する。
スコープは新規登録、アカウント情報変更、パスワードリセット、アカウント状態
確認に限定する。ゲームプレイ用ログインはネイティブクライアントが担当し、
本SPAはログイン画面やゲーム用セッションキーを持たない。

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
        ├── pages/        # RegisterPage, AccountSettingsPage, PasswordResetPage
        ├── components/   # 再利用可能なUI部品（shadcn/ui生成物含む）
        ├── store/        # Zustandストア（アカウント管理状態等）
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

## Phase 1 設計着手（2026-07-22、ADR 0023承認済み）

ゲームプレイ時のログインは本Issueのスコープ外とし、ネイティブクライアント
内のログインフォームが`seed_auth`を直接呼び出す。ブラウザからゲームクライアント
へのディープリンク、カスタムURLスキーム、OSレベルのコールバックは使用しない。
本SPAはアカウントの新規登録、アカウント情報変更、パスワードリセット、
アカウント状態確認に特化する。ゲームプレイ用ログイン機能や、ゲームクライアント
へのチャレンジキー／正規セッションキーの引き渡しは実装しない。

Phase 1では、登録、アカウント管理、パスワードリセットの画面とAPI契約に対する
Redテストを定義し、ゲームプレイログインやネイティブクライアント連携のテストは
作成しない。

## 設計課題

- パスワードそのものは`seed_auth`とこのSPAの間でのみやり取りし、
  `seed_server`には一切渡さない（Adjudicator確認済み、ADR 0018/
  LISS-0147と整合、変更なし）。

## Remaining decisions

- ゲームプレイログインはネイティブクライアント側の別Issue／仕様で扱う。

## English

Phase 1 Red design for a React account-management SPA calling seed_auth.
Shares build-tooling decisions with LISS-0145 (admin UI). Game-play login is
intentionally outside this SPA and is handled by the native client.

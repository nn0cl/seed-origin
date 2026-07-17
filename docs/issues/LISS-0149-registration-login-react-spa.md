# LISS-0149: 登録・ログインReact SPA

- Status: proposed
- Phase: phase-0-design-intake
- Priority: medium
- Depends on: LISS-0146
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

`seed_auth`の`/register`・`/login`を利用する、プレイヤー向け登録・
ログインReact SPAを追加する。

## 背景

管理者UI（LISS-0145）と同じくReactを技術選定として採用する。npm
ツールチェーン選定・配置場所の決定はLISS-0145と共通化できる可能性が
あるため、先に着手する側の決定をもう一方へ適用する想定。

## 設計課題

- 管理者UI（LISS-0145）と同一アプリにするか、別アプリにするか
  （対象ユーザー・バックエンドが異なるため別アプリを推奨するが未決定）。
- ログイン成功後のセッショントークンをゲームクライアントへどう引き継ぐか
  （ブラウザSPAとゲームクライアントが別プロセス/別技術の場合の橋渡し）。

## Remaining decisions

- npmツールチェーン選定はLISS-0145と合わせて決定する。
- ゲームクライアント技術自体は別のNon-Decision（未着手）。

## English

Design intake for a React registration/login SPA calling seed_auth. Shares
open build-tooling decisions with LISS-0145 (admin UI). How the session
token hands off to the (not-yet-decided) game client is an open question.

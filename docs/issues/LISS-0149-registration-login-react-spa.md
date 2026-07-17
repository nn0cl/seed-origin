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

## 解決済みの設計課題（2026-07-18更新）

- npmツールチェーンは**Vite**に決定（ADR 0019でLISS-0145と共通決定）。

## 設計課題

- 管理者UI（LISS-0145）と同一アプリにするか、別アプリにするか
  （対象ユーザー・バックエンドが異なるため別アプリを推奨するが未決定）。
- ログイン成功後のセッションキー（LISS-0146/0147の「先勝ちローテーション」
  設計を参照）をゲームクライアントへどう引き継ぐか（ブラウザSPAと
  ゲームクライアントが別プロセス/別技術の場合の橋渡し）。
- パスワードそのものは`seed_auth`とこのSPA（ゲームクライアントに
  含まれる、またはその手前のWeb登録画面）の間でのみやり取りし、
  `seed_server`には一切渡さない（Adjudicator確認済み、ADR 0018/
  LISS-0147と整合）。

## Remaining decisions

- ゲームクライアント技術自体は別のNon-Decision（未着手）。

## English

Design intake for a React registration/login SPA calling seed_auth. Shares
open build-tooling decisions with LISS-0145 (admin UI). How the session
token hands off to the (not-yet-decided) game client is an open question.

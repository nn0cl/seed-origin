# LISS-0150: 匿名ログイン・ID名寄せの廃止処理

- Status: review
- Phase: docs-supersession-complete
- Related branch: `docs/liss-0150-supersede-alias`
- Priority: medium
- Depends on: LISS-0147（done — PR #12）
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

LISS-0147（ワールドサーバーのセッショントークン検証ログイン）が
mainへ着地した後、匿名ログイン・ID名寄せ関連の資産を正式に
`superseded`として整理する。

## 対象

- `docs/issues/LISS-0123-anonymous-login-alias.md`
- `docs/issues/LISS-0130-identity-persistence-reconciliation.md`
- `docs/issues/LISS-0143-postgres-identity-alias-adapter.md`
- `docs/architecture/adr/0016-identity-alias-persistence-and-review.md`
- `IdentityAliasStore`／`InMemoryIdentityAliasStore`／
  `PostgresIdentityAliasStore`（コード削除は本Issueでは行わない）
- `docs/work-plans/WP-0007-remaining-issues.md`のcanonical表の該当行
- `docs/architecture/README.md`のADR一覧

## 受入条件

- 削除ではなく`superseded`ステータスへの更新（
  `docs/collaboration/local-issue-planning.md`の方針に従う）。
- 実装コードの扱い（削除 or 未使用として保持）を明記する。
- `identity_aliases`テーブルのPostgres上のデータ移行・削除方針
  （既存データがあれば）を決める。

## コード削除タイミングとデータ移行（2026-07-18確定、ADR 0023決定5・6）

- `IdentityAliasStore`系コードは、`seed_auth`（LISS-0146）と
  `seed_admin`のKotlin移行（ADR 0019）の実装完了・検証後に削除する
  （未使用のまま無期限に残さない）。
- 本番データは存在しないため、`identity_aliases`のデータ移行・削除方針の
  検討は不要（本番運用が始まった場合は別途再検討）。

## Docs supersession slice（2026-08-20、Adjudicator承認）

### 実施内容

- LISS-0123 / LISS-0130 / LISS-0143 / ADR 0016 → `superseded`
- WP-0007 canonical 表の 8 / 8.1 行と architecture README を同期
- コード方針: **残置**（pending deletion）
  - `ServerMain` / `SessionRegistry` / `AdminMain` alias review API がまだ参照
  - 削除は ADR 0023 決定5どおり、後続 Issue（seed_auth HTTP + seed_admin
    Kotlin 完了後）で行う
- データ方針: 本番 `identity_aliases` データなし → 移行・削除方針の検討は不要

### 本スライスでやらなかったこと

- `IdentityAliasStore` / Postgres adapter / Admin alias UI のコード削除
- 新規 purge / マイグレーション

## Remaining decisions

- 本スライスの docs 受入は完了。コード削除の着手可否は ADR 0019 /
  LISS-0146 HTTP Adapter 完了後に再検討する。

## English

Formally mark LISS-0123, LISS-0130, LISS-0143, and ADR 0016 as superseded
(not deleted). Retain IdentityAliasStore code until seed_auth and seed_admin
Kotlin migration complete (ADR 0023). No identity_aliases production data
migration is required.

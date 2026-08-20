# LISS-0150: 匿名ログイン・ID名寄せの廃止処理

- Status: proposed
- Phase: phase-0-design-intake
- Priority: medium
- Depends on: LISS-0147
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

LISS-0147（ワールドサーバーのセッショントークン検証ログイン）が
mainへ着地した後、匿名ログイン・ID名寄せ関連の資産を正式に
`superseded`として整理する。

## 対象

- `docs/issues/LISS-0123-anonymous-login-alias.md`
- `docs/issues/LISS-0130-identity-persistence-reconciliation.md`
- `docs/architecture/adr/0016-identity-alias-persistence-and-review.md`
- `IdentityAliasStore`／`InMemoryIdentityAliasStore`／
  `PostgresIdentityAliasStore`（`SessionRegistry`からの参照を含め、
  コードとして削除するか、残すが未使用であることを明記するかは
  本Issueで決定する）
- `docs/work-plans/WP-0007-remaining-issues.md`のcanonical表の該当行
- `docs/architecture/README.md`のADR一覧・Non-Decision一覧

## 受入条件（ドラフト）

- 削除ではなく`superseded`ステータスへの更新（
  `docs/collaboration/local-issue-planning.md`の方針に従う）。
- 実装コードの扱い（削除 or 未使用として保持）を明記する。
- `identity_aliases`テーブルのPostgres上のデータ移行・削除方針
  （既存データがあれば）を決める。

## Remaining decisions

- コード削除の是非、既存`identity_aliases`データの扱いは未決定。

## English

Once LISS-0147 lands, formally mark LISS-0123, LISS-0130, and ADR 0016 as
superseded (not deleted) per this repository's local-issue-planning
convention, and decide the fate of the now-unused IdentityAliasStore code
and any existing identity_aliases data.

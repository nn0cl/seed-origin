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

## コード削除タイミングとデータ移行（2026-07-18確定、ADR 0023決定5・6）

- `IdentityAliasStore`系コードは、`seed_auth`（LISS-0146）と
  `seed_admin`のKotlin移行（ADR 0019）の実装完了・検証後に削除する
  （未使用のまま無期限に残さない）。
- 本番データは存在しないため、`identity_aliases`のデータ移行・削除方針の
  検討は不要（本番運用が始まった場合は別途再検討）。

## Remaining decisions

- 前提（削除タイミング、データ移行不要）は確定済み。他のissue
  （LISS-0146/0147/0149）の検討が進んだ後に、本Issue自体の着手可否を
  改めて検討する（Adjudicator指示、2026-07-18）。

## English

Once LISS-0147 lands, formally mark LISS-0123, LISS-0130, and ADR 0016 as
superseded (not deleted) per this repository's local-issue-planning
convention, and decide the fate of the now-unused IdentityAliasStore code
and any existing identity_aliases data.

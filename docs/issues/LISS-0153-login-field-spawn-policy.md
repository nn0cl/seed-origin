# LISS-0153: Login 後の Field スポーン規則（恒久ポリシー）

## Metadata

- Local issue ID: LISS-0153
- GitHub issue:
- Status: proposed
- Phase: phase-0-design-intake
- Type: feature + world
- Priority: high
- Initial planning size: M
- Current planning size: M
- Reclassification reason:
- Owner/agent:
- Related branch:
- Parent: LISS-0152
- Depends on: LISS-0152
- Related: LISS-0072, LISS-0099, LISS-0102, LISS-0122, LISS-0125

## Summary

Login 成功後にプレイヤーを Field へ配置する恒久規則（初期座標、HP/MP、
map、PlayerId と sessionId の将来的な分離、切断時の退出、再接続時の復元）
を仕様化する。LISS-0152 が載せた仮の origin 配置を置き換える。

## Acceptance Notes

- 初期 pose・初期 Status・スポーン地点の選択規則が EARS/Gherkin で固定される。
- Login の `session.internalId` と Field の `PlayerId` / Snapshot
  `player.<i>.session` の対応が明示される。
- 切断・logout 時の Field 退出、再接続時の復元または再スポーンが明示される。
- RequestSnapshot ワイヤ Command は本 Issue の必須範囲ではない（LISS-0154）。

## Adjudicator Decision Points

- 初期座標を origin 以外にするか（ゾーン、最後のログアウト地点、衝突回避）。
- 初期 HP/MP。現行の仮実装は Snapshot テストと同じ `Status()`（0, 0）。
- `PlayerId` を `session.internalId` と同一キーのままにするか。
- 同一 origin への複数同時スポーンを許容するか。

## Context

- Included: Field 配置、join Snapshot の公開 pose、session 束縛との対応。
- Omitted: auth（LISS-0146–0150）、20 Hz フル Snapshot、RequestSnapshot
  Command（LISS-0154）、クライアント予測本体。
- Assumptions: protocol version 1。完全/差分は同一 WorldUpdate 列。

## AI Planning Records

### AIP-0153-001

- Status: accepted
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-19
- Planning size: M
- Intended execution route: Architecture/Feature Path after Adjudicator
  answers spawn policy. Not implemented on this record.
- Intended scope: durable spawn/despawn rules only.
- Estimated token range: 20k–50k
- Estimated token midpoint: 35k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: Field, session lifecycle, snapshot payload, one spec.
- Assumptions: temporary origin spawn already landed under LISS-0152.
- Confidence: medium
- Revises:
- Revision reason:
- Superseded by:

## Work Notes

Opened because LISS-0152 はスポーン規則を仕様化しない。join Snapshot の
`player.count=0` を止めるため、0152 側は既存テスト慣例（`PlayerId` =
sessionId、`Position(id, 0, 0, 0)`、`Status()`）で仮配置する。

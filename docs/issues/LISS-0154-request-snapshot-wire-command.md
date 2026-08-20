# LISS-0154: RequestSnapshot ワイヤ Command

## Metadata

- Local issue ID: LISS-0154
- GitHub issue:
- Status: review
- Phase: phase-2-green
- Type: feature + recovery
- Priority: high
- Initial planning size: M
- Current planning size: M
- Reclassification reason:
- Owner/agent: Cursor Auto
- Related branch: `feature/liss-0152-client-side-prediction`
- Parent: LISS-0122
- Depends on: LISS-0121, LISS-0122, LISS-0152
- Related: LISS-0128, LISS-0089
- Blocks: LISS-0128 (transport shell still owns reconnect socket I/O)

## Summary

欠番・再接続でクライアント内部の RequestSnapshot フラグを、サーバーが
既存の完全取得（Snapshot）を 1 枚積む **ワイヤ Command** にする。
protocol version は 1 のまま。別チャンネルや 20 Hz フル Snapshot は出さない。

## Acceptance Notes

- Command type `RequestSnapshot`、空 payload。session は接続済み内部 ID。
- sequence gap または `beginReconnect` のあと、ログイン済みクライアントが
  この Command を組み立てる。
- サーバーはアクティブセッションだけ受理し、そのティックで
  `capturePublicSnapshot` / `appendSnapshot` を **最大 1 枚**（join 要求と合算）。
- 公開 `player.*` は join 時と同じ。本人 `local.*` は既存
  `copyWorldUpdateForSession`。未ログイン接続には出さない。
- Snapshot 適用後に Event 再開、期待 sequence は Snapshot+1。
- 差分 `movement=` と混同しない。再接続ソケット I/O は LISS-0128。

## Adjudicator Decision Points

- None remaining for this slice (Adjudicator 2026-08-19: empty payload,
  same WorldUpdate sequence, coalesce to one Snapshot per tick, no UDP/AoI).

## Context

- Included: Command codec, dispatcher, rate limit 1/session/tick, snapshot
  capture trigger, client command builder, Event skip while waiting.
- Omitted: auth（LISS-0146–0150）、20 Hz フル Snapshot、LISS-0153 spawn、
  reconnect TCP I/O、UDP、AoI、別 sequence。

## AI Planning Records

### AIP-0154-001

- Status: accepted
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-19
- Planning size: M
- Intended execution route: Feature Path AT-TDD on the CSP branch.
- Intended scope: RequestSnapshot wire Command and one Snapshot per tick.
- Estimated token range: 20k–50k
- Estimated token midpoint: 35k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: protocol enum, dispatcher, receiver, CTest, specs.
- Assumptions: join Snapshot path already exists; protocol version stays 1.
- Confidence: high
- Revises:
- Revision reason:
- Superseded by:

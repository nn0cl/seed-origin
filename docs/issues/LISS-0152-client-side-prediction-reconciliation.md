# LISS-0152: ローカル移動の client-side prediction と server reconciliation

## Metadata

- Local issue ID: LISS-0152
- GitHub issue:
- Status: in_progress
- Phase: phase-2-green
- Type: feature + netcode
- Priority: high
- Initial planning size: L
- Current planning size: L
- Reclassification reason:
- Owner/agent: Cursor Auto
- Related branch: `feature/liss-0152-client-side-prediction`

## Summary

サーバー権威・固定ティック（20 Hz）を維持したまま、ローカルプレイヤー移動
だけを即時予測し、権威 pose と未 ack 入力の replay で再収束する。

## Acceptance Notes

- クライアントは input のみを送り、position を権威値として送らない。
- Move payload にプレイヤーごとの単調増加 `clientInputSequence` を載せる。
- サーバーは重複・古い入力を二重適用せず、ack に
  `lastProcessedInputSequence` と権威 `x,y,z` / `worldTick` を
  **本人セッション限定**で返す。公開移動は既存 `movement=` delta の
  全接続 fan-out。WorldUpdate.sequence にギャップを作らない。
- クライアントは pending ring buffer を保持し、ack 後に権威状態へ戻して
  未 ack 入力を replay する。
- 予測 state と render state を分離する。小さい誤差は 100–200 ms で平滑化、
  大きい誤差と snapshot/teleport は snap。
- 他プレイヤー・NPC・戦闘結果は予測しない。
- WorldUpdate sequence と client input ack を混同しない。

## Dependencies

- Parent: LISS-0125
- Depends on: LISS-0121, LISS-0125
- Blocks:
- Related: LISS-0082, LISS-0122, WP-0006

## Adjudicator Decision Points

- Decided 2026-08-17: `movementAck`（`lastProcessedInputSequence` と本人の
  権威 pose）は本人セッション限定。他人の公開移動は既存 `movement=` Event
  の broadcast。ack を別の sequenced WorldUpdate として他人に省略しては
  ならない。
- Decided 2026-08-17: protocol version は 1 のまま。

## Context

- Included: Move payload、MovementIntent、WorldFrameApplier / UpdateBuilder、
  ClientWorldUpdateReceiver、ClientWorldSnapshotApplier、Field 移動積分。
- Omitted: 未コミット auth 変更、レンダラー、他プレイヤー予測、戦闘予測、
  wire format 全面刷新。
- Assumptions: 既存 `dx,dy,dz` は `clientInputSequence=0` の互換経路。
  サーバー内部 `MovementIntent.sequence` / `WorldUpdate.sequence` は
  client input sequence ではない。

## AI Planning Records

### AIP-0152-001

- Status: accepted
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-17
- Planning size: L
- Intended execution route: Feature Path Phase 1 Red then Phase 2 Green in this
  session, because the Adjudicator requested full test-then-implementation.
- Intended scope: local-player movement prediction and reconciliation only.
- Estimated token range: 80k–160k
- Estimated token midpoint: 120k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: protocol/docs plus server and client netcode across ~20
  source files and new tests.
- Assumptions: C++20 as in CMake; payload additive; worktree isolated from
  uncommitted auth files.
- Confidence: medium
- Revises:
- Revision reason:
- Superseded by:

## References

- `docs/specs/client-side-prediction-v1.md`
- `docs/specs/network-protocol-v1.md`
- `docs/architecture/seed-implementation.md`

## Work Notes

Adjudicator requested Phase 1 then Phase 2 in one session. Tests were written
from the Gherkin spec before production code.

Owner-only ack (2026-08-17): public `movement=` stays a global sequenced
fan-out. Authoritative pose + `lastProcessedInputSequence` are attached only
to the owner's copy of that same sequence. `ClientWorldUpdateReceiver` binds
the local session and ignores foreign acks.

Work lives on branch `feature/liss-0152-client-side-prediction` in worktree
`/Users/nn0cl/Documents/git/seed-origin-prediction` so uncommitted auth files
on `feature/ue-client-mockup-delivery-brief` were not touched.

## Verification

- Owner-only ack subset: EXIT 0 via isolated driver linking the rebuilt
  `seed_tests` objects (`personalizes_owner_copy_without_changing_sequence`,
  `acks_monotonic_client_input_sequences`,
  `ignores_foreign_session_movement_ack`,
  `reconciles_from_owner_copy_of_public_movement`,
  `other_session_keeps_sequence_without_owner_ack`, plus prior CSP tests).
- Full `seed_tests` still hits the pre-existing main failure
  `dispatches_pending_commands_in_fifo_order` (Move with sessionId 0) before
  later cases. Not introduced by this change.

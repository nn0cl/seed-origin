# LISS-0152: ローカル移動の client-side prediction と server reconciliation

## Metadata

- Local issue ID: LISS-0152
- GitHub issue:
- Status: review
- Phase: phase-2-green-complete
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
- 完全取得（Snapshot）と差分取得（Event）は同じ WorldUpdate 列。
  完全は join / sequence 欠番 / 再接続で、止まっている人も含む公開
  pose を置き換える。差分は 20 Hz Event で動いた人だけ（非本人
  `movement=` の `;x=;y=;z=`）。欠番は Event から推測しない。
- 20 Hz はティックと差分 Event の周期。フル Snapshot を 20 Hz では
  出さない。RequestSnapshot ワイヤ Command は LISS-0154。Login 後の
  Field 配置は仮の origin（LISS-0153 で恒久規則に置き換え）。
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
- Related: LISS-0082, LISS-0122, LISS-0154, WP-0006

## Adjudicator Decision Points

- Decided 2026-08-17: `movementAck`（`lastProcessedInputSequence` と本人の
  権威 pose）は本人セッション限定。他人の公開移動は既存 `movement=` Event
  の broadcast。ack を別の sequenced WorldUpdate として他人に省略しては
  ならない。
- Decided 2026-08-17: protocol version は 1 のまま。
- Decided 2026-08-19: 完全取得と差分取得は同じ WorldUpdate sequence
  列（別チャンネル／別 sequence にしない）。完全は Snapshot
  （join / 欠番 / 再接続、静止者含む公開 pose の置き換え）。差分は
  20 Hz Event（動いた人だけ、非本人 `;x=;y=;z=`）。欠番は Event から
  推測せず再完全取得。本人 ack は同じ sequence の本人コピーだけ。
  20 Hz はティックと差分周期でありフル Snapshot 周期ではない。
  RequestSnapshot ワイヤ Command は LISS-0154。Login 後の Field
  配置は仮の origin（`PlayerId == session.internalId`、`(0,0,0)`、
  `Status()`）。恒久スポーンは LISS-0153。独立 ADR は切らない（既存の
  グローバル sequence 運用の確認であり、ADR 0001
  系の新規技術選定ではない。記録は本 Issue と
  `docs/specs/client-side-prediction-v1.md` /
  `docs/specs/network-protocol-v1.md`）。

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

Full vs delta (2026-08-19): Snapshot and Event stay one sequence column.
Idle remotes are replaced on Snapshot and omitted from 20 Hz movement
Events. Sequence gaps take another Snapshot; they are not reconstructed
from Events. RequestSnapshot Command is LISS-0154. Post-Login Field
placement uses the temporary origin convention; durable spawn is
LISS-0153.

Work lives on branch `feature/liss-0152-client-side-prediction` in worktree
`/Users/nn0cl/Documents/git/seed-origin-prediction` so uncommitted auth files
on `feature/ue-client-mockup-delivery-brief` were not touched.

## Verification

- 2026-08-20: Full `./build/seed_tests` in worktree
  `/Users/nn0cl/Documents/git/seed-origin-prediction` — **EXIT 0** (entire
  suite green, including combat spell enqueue cases previously noted as
  aborting on this branch).
- Owner-only ack, full-vs-delta, login Field placement, RequestSnapshot,
  Disconnect lifecycle, reconnect I/O: covered by Gherkin-mapped CTest cases
  (`ClientPredictionSyncTest`, `ClientTransportShellTest`,
  `FieldSessionPresenceTest`, `RequestSnapshotCommandTest`,
  `DisconnectCommandHandlerTest`, `RemotePlayerPoseStoreTest`,
  `LocalMovementPredictorTest`, and related WorldFrame tests).
- Production server tick loop (`src/ServerMain.cpp`) publishes owner acks and
  coalesced join/reconnect Snapshots on the shared WorldUpdate sequence.
- Client prediction/render separation: `LocalMovementPredictor` and
  `RemotePlayerPoseStore` implement 150 ms smooth / 5.0 snap thresholds
  (`PREDICTION_CORRECTION_SECONDS`, `PREDICTION_SNAP_DISTANCE`).
- Completion handoff:
  `docs/collaboration/traces/2026-08-20-liss-0152-completion-handoff.md`.

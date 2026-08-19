# LISS-0153: Login 後の Field スポーン規則（恒久ポリシー）

## Metadata

- Local issue ID: LISS-0153
- GitHub issue:
- Status: in_progress
- Phase: phase-2-green
- Type: feature + world
- Priority: high
- Initial planning size: M
- Current planning size: M
- Reclassification reason:
- Owner/agent:
- Related branch: feature/liss-0152-client-side-prediction
- Parent: LISS-0152
- Depends on: LISS-0152
- Related: LISS-0072, LISS-0099, LISS-0102, LISS-0122, LISS-0125, LISS-0146–0150

## Summary

Login 成功後の Field 配置を、設定可能な初期 pose/HP/MP と 4 役割の ID
モデルで固定する。session を永続 PlayerId として使わない。

## Identity roles

| 役割 | 発行・寿命 | ワイヤ | UI |
| --- | --- | --- | --- |
| 認証 PlayerId | 認証サーバー（早期はポート stub）。再接続で不変 | 出さない | 出さない |
| session ID | `session.internalId`。再接続のたびに更新 | ヘッダ / Snapshot `player.<i>.session` / `movement=session:` | 出さない |
| ゲーム内ID | Field `Player::getPlayerId()`。Attack/CastSpell `targetId` | Snapshot `player.<i>.id` | 出さない |
| PlayerName | 表示専用。運営が付与（早期は spawn 設定 / テストスタブ / operator setter）。Login claimedId ではない | Snapshot `player.<i>.name`（必須・非空） | これだけ見せる |

ゲーム内ID は既存の Field キー / `targetId` を寄せたワールド側の正の int64
であり、認証 PK でも session でもない。公開安定 UUID は発行しない。

## Acceptance Notes

- 初期 pose / HP / MP は `LoginFieldSpawnSettings`（または
  `LoginFieldSpawnPolicy`）で設定できる。
- 未設定時の早期デフォルト: `(0,0,0)`, HP 10, MP 10。
- スポーン HP/MP の max 早期値: 1024, 1024（コードにゲーム上限定数は無かった。
  `Status::gainHp` の既存飽和は `long` 最大値のまま。Adjudicator は 1024 を
  提案し、配置経路だけそれに合わせた）。
- 再接続は新しい session を同じ認証 PlayerId の Field 実体へ結び直す。
  disconnect は session を外し、公開 pose から外す。実体は unset しない。
- 同じ PlayerName の第二配置は失敗する（Field 上の完全一致）。
- 空の PlayerName での配置と Snapshot 公開は失敗する。
- プレイヤー経路では改名できない。運営の `operatorSetPlayerName`（または spawn 設定）だけが名前を付ける。
- ゲーム内ID と認証 PlayerId は再接続でも不変。session だけ更新する。
- RequestSnapshot ワイヤ Command は必須範囲ではない（LISS-0154）。

## Adjudicator decisions (2026-08-19)

- 初期座標・HP/MP は設定可能。早期 (0,0,0) / 10,10。max 未決なら 1024,1024。
- 切断後に戻るのは元のプレイヤー実体。session は更新。認証 ID は非公開。
- 操作はゲーム内ID。表示はネームのみ。ネームは重複不可。早期は完全一致。
- 他クライアントのリモート追跡キーはゲーム内ID（`player.<i>.id`）。
  session は通信。HUD は PlayerName。認証 PlayerId は公開しない。
- seed-auth 本体は本 worktree から触らない。発行はポート。早期 stub。

## Adjudicator decisions (2026-08-20)

- 空ネームは配置も公開も不可。
- キャラクターネームは運営のみ。プレイヤー改名コマンドは作らない。
  Login claimedId を表示名に流用しない。
- 認証 PlayerId とゲーム内ID はプレイヤーから変更不可。再接続でも不変。
  session だけ接続ごとに更新。

## Ambiguities

- PlayerName の大文字小文字・空白の正規化。
- アカウント横断の一意帳票（auth 側）。ポート契約は「名前は一意」とし、
  本物の帳票は LISS-0146–0150 後続。
- ゾーン・ログアウト地点・衝突回避スポーン。
- 運営 UI 全体と権限モデル（早期は関数レベルの operator setter のみ）。

## Context

- Included: Field 配置、join Snapshot、session 束縛、ゲーム内ID、表示名。
- Omitted: seed-auth 実装、20 Hz フル Snapshot、別チャンネル。
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
- Intended execution route: Feature Path AT-TDD
- Intended scope: spawn settings + identity split + unique PlayerName
- Estimated token range: 20k–50k
- Estimated token midpoint: 35k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: Field, presence, snapshot payload, tests, spec.
- Assumptions: LISS-0152 temporary origin already on the branch.
- Confidence: medium

### AIP-0153-002

- Status: accepted
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-20
- Planning size: M
- Intended execution route: Feature Path AT-TDD
- Intended scope: remote store keyed by gameplay id through client apply
- Estimated token range: 15k–35k
- Estimated token midpoint: 25k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: RemotePlayerPoseStore, snapshot applier, receiver tests.
- Assumptions: spawn/identity split from AIP-0153-001 already committed.
- Confidence: medium

### AIP-0153-003

- Status: accepted
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-20
- Planning size: M
- Intended execution route: Feature Path AT-TDD
- Intended scope: operator-only non-empty PlayerName; immutable ids except session
- Estimated token range: 15k–35k
- Estimated token midpoint: 25k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: presence, snapshot builder/applier, identity tests.
- Assumptions: AIP-0153-001 and AIP-0153-002 already committed.
- Confidence: medium

## Work Notes

既存 Attack/CastSpell の `targetId` と Field `playerList` キーがゲーム内ID。
認証 ID は `Player::authPlayerId`。session は `Field` の束縛マップ。
クライアント `RemotePlayerPoseStore` のキーもゲーム内ID。session は
movement 差分の引き当て。`RemotePlayerPose::name` が HUD 表示。

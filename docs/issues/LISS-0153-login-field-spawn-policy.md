# LISS-0153: Login 後の Field スポーン規則（恒久ポリシー）

## Metadata

- Local issue ID: LISS-0153
- GitHub issue:
- Status: done
- Phase: adjudicator-policy-closure-complete
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
- 同じ PlayerName の第二配置は失敗する（trim 後の完全一致、大小文字はそのまま）。
- 空の PlayerName での配置と Snapshot 公開は失敗する。前後 ASCII 空白だけも
  trim 後 empty として空と同じ。
- 名前の一意は Field 在籍に依存しない。ログアウトや `unsetPlayer` でも解放しない。
  早期はメモリ上の claimed-name レジストリ。auth 永続帳票は LISS-0146–0150。
- プレイヤー経路では改名できない。運営の `operatorSetPlayerName`（または spawn 設定）だけが名前を付ける。
- 運営が同一 auth PlayerId を改名したとき、trim 後の旧名は claimed から外れ、新名がその auth の独占になる。失敗した改名は旧名を残す。
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
- 空白のみの名前は空。trim は空判定と一意比較のため。Unicode / case fold はしない。
- 名前の一意はログアウト後も維持。Field unset でも解放しない。早期はメモリ
  claimed-name レジストリ。
- 運営改名は旧名をレジストリから外し、新名を独占する。プレイヤー改名は不可。
  空・空白のみは不可。一意は trim 後完全一致。ID は不変。

## Adjudicator decisions (2026-08-20, durable policy closure)

Adjudicator は推奨案を承認。以下 4 点は恒久ポリシーとして確定。

### 1. PlayerName 正規化 — resolved

- **早期（本 Issue / world 側）:** trim 後の完全一致。大小文字は区別する（`Hero` ≠ `hero`）。
- **第2段（auth 帳票）:** Unicode NFC 正規化と case-insensitive 一意は LISS-0146–0150
  の auth 永続帳票 Phase 2。world の trim/exact-match ルールはそのまま維持。
- **Follow-up:** LISS-0146–0150（または専用 follow-up issue）で auth 側正規化ポリシーと
  world 検証契約を定義する。

### 2. スポーン位置 — resolved

- **早期:** `LoginFieldSpawnSettings`（または `LoginFieldSpawnPolicy`）のみ。未設定時デフォルト
  は origin `(0,0,0)`、HP/MP `10,10`（max `1024,1024`）を維持。
- **第2段:** ゾーン別 spawn・ログアウト地点復帰は auth spawn port（LISS-0146–0150 後続）
  導入後の拡張。`LoginFieldSpawnSettings` 拡張 vs 別 policy はその段階で再設計。
- **未決（本 Issue 外）:** 衝突回避スポーンの優先順位は未決。同一 origin への複数配置は
  早期では許容する。

### 3. auth 側名前帳票 — resolved

- **早期:** world サーバーの in-memory claimed-name レジストリは**暫定**。
- **正（authoritative）:** seed-auth（LISS-0146–0150）がアカウント横断の global 一意帳票。
- **移行:** Login 時に auth が PlayerName を返す port へ移行し、world は受け取った名前の
  検証（trim 後非空・trim 後 exact-match 一意）のみ行う。world 主導の帳票は廃止。
- **参照（main worktree）:** `docs/specs/player-authentication-flow-v1.md`、
  `docs/architecture/adr/0023-player-auth-session-flow-details.md`。

### 4. 運営 UI / 権限 — resolved

- **早期:** `operatorSetPlayerName`、spawn 設定、テスト/CLI stub。ワイヤ上のプレイヤー改名
  コマンドは作らない（維持）。
- **本番:** LISS-0144 等の admin HTTP API / LISS-0145 admin UI へ移行。
- **権限モデル:** 別 ADR（未作成）。本 Issue では関数レベル setter の delivery 面のみ確定。

## Implemented on branch (decided items only)

以下は Adjudicator 既決（2026-08-19 / 2026-08-20）に基づき
`feature/liss-0152-client-side-prediction` に実装済み。本 Issue の
**proposed** 範囲は未決ポリシーと恒久化のみ。

- `LoginFieldSpawnSettings` による初期 pose / HP / MP（未設定時 `(0,0,0)` / 10,10、max 1024）
- 4 役割 ID 分離（認証 PlayerId / session / ゲーム内ID / PlayerName）
- 運営のみ PlayerName 付与（spawn 設定 / operator setter / テスト stub）
- trim 後 empty 拒否、trim 後完全一致の一意（case fold なし）
- claimed-name レジストリ（logout / Field unset 後も解放しない）
- 運営改名で旧名 release → 新名 claim
- 再接続: 新 session を同一 auth + ゲーム内ID 実体へ rebind
- Disconnect: session 除去・公開 pose 除去、実体は unset しない
- リモート追跡キー = ゲーム内ID（`RemotePlayerPoseStore`）
- E2E: disconnect → reconnect で RequestSnapshot がサーバーに届く
  （`loopback_disconnect_ends_session_and_resets_client_auth`）

## Resolved adjudicator questions (2026-08-20)

| # | 質問 | 決定 | 記載箇所 |
| --- | --- | --- | --- |
| 1 | PlayerName 正規化 | 早期 trim + exact-match（case-sensitive）。NFC / case-insensitive は auth Phase 2 | 上記 §1 |
| 2 | スポーン位置 | 早期 `LoginFieldSpawnSettings` + `(0,0,0)`。ゾーン/ログアウトは第2段。衝突回避未決・同一 origin 許容 | 上記 §2 |
| 3 | auth 名前帳票 | world claimed-name は暫定。正は seed-auth。Login port で PlayerName 返却、world は検証のみ | 上記 §3 |
| 4 | 運営 UI / 権限 | 早期 `operatorSetPlayerName` + stub。本番 LISS-0144 等。権限は別 ADR | 上記 §4 |

## Ambiguities (remaining, out of scope for LISS-0153)

- 衝突回避スポーンの優先順位と設定 surface。
- 運営 rename の権限モデル（別 ADR）。
- auth Phase 2 の Unicode NFC / case-insensitive 正規化の具体契約（LISS-0146–0150 follow-up）。

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

### AIP-0153-004

- Status: accepted
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-20
- Planning size: M
- Intended execution route: Feature Path AT-TDD
- Intended scope: trim-empty names; claimed-name registry survives unset
- Estimated token range: 10k–25k
- Estimated token midpoint: 18k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: presence tests, PlayerName trim helper, registry.
- Assumptions: AIP-0153-003 already committed.
- Confidence: medium

### AIP-0153-005

- Status: accepted
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-20
- Planning size: M
- Intended execution route: Feature Path AT-TDD
- Intended scope: operator rename releases previous claimed name
- Estimated token range: 8k–20k
- Estimated token midpoint: 14k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: presence tests, claimed-name registry, spec/LISS notes.
- Assumptions: AIP-0153-004 already committed.
- Confidence: high

## References

- `docs/specs/client-side-prediction-v1.md` — Login Field placement（LISS-0153）
- `docs/work-plans/WP-0010-client-side-prediction.md` — issue graph
- `docs/architecture/adr/0023-player-auth-session-flow-details.md`（`seed-origin` main）
- `docs/specs/player-authentication-flow-v1.md`（`seed-origin` main）
- `docs/architecture/adr/0024-language-specific-test-layouts.md`（`seed-origin` main、テスト配置のみ）

## Work Notes

既存 Attack/CastSpell の `targetId` と Field `playerList` キーがゲーム内ID。
認証 ID は `Player::authPlayerId`。session は `Field` の束縛マップ。
クライアント `RemotePlayerPoseStore` のキーもゲーム内ID。session は
movement 差分の引き当て。`RemotePlayerPose::name` が HUD 表示。

- 2026-08-20: Adjudicator approved durable policy closure; issue `review` → `done`.

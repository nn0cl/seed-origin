# Network Protocol v1

## Purpose

クライアントとゲームサーバー間で、検証可能なCommandを運ぶ最小のバイナリ契約。

## Frame layout

| Offset | Size | Field | Encoding |
| ---: | ---: | --- | --- |
| 0 | 2 | protocol version | unsigned, big endian |
| 2 | 2 | command type | unsigned, big endian |
| 4 | 8 | session ID | signed 64-bit, big endian |
| 12 | 4 | payload length | unsigned, big endian |
| 16 | N | payload | opaque command-specific bytes |

`N`は`MAX_COMMAND_PAYLOAD`以下でなければならない。Version 1のCommand typeはLogin、Move、Chat、Attack、CastSpell、Disconnect、RequestSnapshotである。RequestSnapshotのpayloadは空。session IDは接続が持つ正の内部ID（Login以外と同じ）。protocol versionは1のまま（Command型追加はv2に上げない）。

Loginだけsession ID 0を許可し、それ以外はサーバーが発行した正の内部IDを要求する。クライアントは攻撃結果、魔法結果、座標、時刻を権威値として送信しない。

TCPの部分read/write、切断、タイムアウト、再送はConnection/transport adapterで処理し、codecは完全なframe単位だけを受け取る。codecはWorld状態を変更しない。

## Login payloadの意味変更予定（2026-07-18、未実装）

現在Loginのpayloadは自己申告ニックネーム（自由記述文字列）として扱われる
（`SessionRegistry::isValidClaimedId`が文字種・長さのみ検証し、資格情報の
検証は行わない）。ADR 0018（登録制プレイヤー認証）・LISS-0147により、
Loginのpayloadは**`seed_auth`が発行した一回限りのセッションキー**に
意味が変わる予定。フレームレイアウト自体（16バイトヘッダ+payload）は
変更しないが、payloadの内容とサーバー側の検証ロジックが置き換わる。
LISS-0147がmainへ着地するまでは、この節は将来の変更点の記録であり、
現在の実装（自己申告ニックネーム）を正とする。

## Move payload (local-player prediction)

Move の payload は入力だけを運ぶ。クライアントは座標・時刻を権威値として送らない。

| Form | Example | Meaning |
| --- | --- | --- |
| Sequenced | `3:1,0,0` | `clientInputSequence:dx,dy,dz` |
| Legacy | `1,2,3` | `dx,dy,dz` with `clientInputSequence` 0 |

`clientInputSequence` はプレイヤーごとにクライアントが発行する単調増加番号（1, 2, 3, …）であり、サーバー内部の `MovementIntent.sequence` や `WorldUpdate.sequence` とは別空間である。legacy 形式は互換のため残し、予測 ack 追跡には入れない。

サーバーは当該プレイヤーについて最後に処理した `clientInputSequence` と権威座標を、**本人セッションの送信コピーにだけ**付ける。公開 `movement=` Event はログイン済み全接続へ同じ `WorldUpdate.sequence` で fan-out する。完全取得（Snapshot）と差分取得（Event）もその同じ sequence 列であり、別チャンネルにしない。`dx,dy,dz` は発生記録であり、クライアントはそれを積分して他人の絶対座標を捏造してはならない。他人の権威 pose は、完全取得では Snapshot の `player.*`（止まっている人も含む公開 pose の置き換え）、差分取得では動いた人だけの非本人 `movement=` に付ける `;x=;y=;z=` として届く。止まっている人は 20 Hz Event では再送しない。欠番は Event から推測せず、もう一度完全取得する。`movementAck` / `lastProcessedInputSequence` を別の sequenced WorldUpdate として他人にだけ省略してはならない。20 Hz はティックと差分 Event の周期であり、フル Snapshot を 20 Hz では出さない。protocol version は 1 のまま。

公開 Event:

```text
movement=session:<id>;dx=<f>;dy=<f>;dz=<f>;clientInputSequence=<u64>;inputSequence=<u64>
```

本人コピーだけが同じ sequence のまま次を追加する:

```text
;x:<f>;y:<f>;z:<f>;worldTick:<u64>;lastProcessedInputSequence:<u64>
```

非本人コピーは同じ sequence のまま、公開絶対 pose だけを追加してよい（owner ack は付けない）:

```text
;x=<f>;y=<f>;z=<f>
```

Standalone の `movementAck=` はテストと防御用にパース可能だが、グローバル sequence を進める別 Event としては配信しない。

Snapshot 再同期時は任意で次を含めてよい（本人予測の rebase 用。公開リモート pose ではない）:

```text
local.x=<f>;local.y=<f>;local.z=<f>;local.lastProcessedInputSequence=<u64>
```

Snapshot は公開プレイヤー pose を載せる（完全取得。止まっている人も含む置き換え）:

```text
player.count=<n>;player.<i>.session=<id>;player.<i>.x=<f>;player.<i>.y=<f>;player.<i>.z=<f>;player.<i>.id=<gameplayId>;player.<i>.name=<name>
```

`session` は接続同期用でありゲーム内 UI には出さない。`id` は Attack/CastSpell の `targetId` と同じゲーム内ID。`name` は表示専用（空なら省略してよい）。認証 PlayerId は載せない。

Login 後の Field 配置（初期 pose/HP/MP、session とゲーム内ID の分離、再接続）は LISS-0153。詳細は `docs/specs/client-side-prediction-v1.md`。

## Server frame and client presentation contract

- The authoritative server advances the World at 20 Hz; one simulation frame is 50 ms.
- Commands received before a frame boundary are assigned a monotonic receive sequence and are processed in that order.
- Commands received while a frame is being processed are deferred to the next frame.
- A server update contains a `worldTick` and a monotonically increasing `sequence`.
- Snapshot and Event share that same `sequence` column. They are the full
  and delta modes of one WorldUpdate stream, not separate channels or
  sequence spaces. Protocol version stays 1.
- A Snapshot is the full fetch: join, sequence gap, and reconnect. It
  replaces public poses for every present player, including players who
  did not move.
- An Event is an occurrence for presentation or audit, and (for movement)
  the 20 Hz delta: only movers are published. Non-owner copies of
  `movement=` may carry that mover's post-tick public pose as
  `;x=;y=;z=`. Stationary players are not resent on the Event.
- Clients must not infer omitted players, missing sequences, or absolute
  coordinates from an Event occurrence record (`dx,dy,dz`) alone. A gap
  requests another Snapshot (full fetch) rather than reconstructing state
  from Events. Explicit public `x= y= z=` on a movement Event is the
  delta pose for that mover only.
- Clients may render at a different rate and interpolate public poses
  received from Snapshot (full) or from those explicit Event poses
  (delta). Client-side effect playback must not delay or overwrite
  authoritative state.
- Snapshot/Event identifiers are used to discard duplicates and detect gaps.
- `lastProcessedInputSequence` (owner ack) is attached only to the owner's
  copy of the same `sequence`.
- 20 Hz is the world tick and the delta Event period. The server does not
  emit a full Snapshot at 20 Hz.
- RequestSnapshot (command type 7) is the wire Command for another full
  fetch after a sequence gap or reconnect. Payload is empty; session ID
  comes from the connection. The server accepts it only for an active
  session, coalesces pending requests with join Snapshot needs, and
  appends at most one Snapshot on the shared WorldUpdate.sequence that
  tick (`capturePublicSnapshot` / `appendSnapshot`). Public `player.*`
  matches join. Owner `local.*` uses existing `copyWorldUpdateForSession`.
  Unauthenticated connections are not published to. Per-session limiter
  admits one RequestSnapshot per world tick. Reconnect socket I/O (this
  slice of LISS-0128) sends that Command on the existing POSIX TCP
  connection after `beginReconnect` and an accepted Login. It does not
  add UDP, a second sequence, or a 20 Hz full-Snapshot stream.
- Login success places the session on the Field using configurable spawn
  (early default pose `(0,0,0)`, HP/MP `10,10`, spawn max `1024,1024`).
  Snapshot lists connection `session`, gameplay `id` (Attack `targetId`),
  and display `name`. Auth PlayerId is not on the public wire. See
  LISS-0153 and `docs/specs/client-side-prediction-v1.md`.

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

`N`は`MAX_COMMAND_PAYLOAD`以下でなければならない。Version 1のCommand typeはLogin、Move、Chat、Attack、CastSpell、Disconnectである。

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

サーバーは当該プレイヤーについて最後に処理した `clientInputSequence` と権威座標を、**本人セッションの送信コピーにだけ**付ける。公開 `movement=` Event はログイン済み全接続へ同じ `WorldUpdate.sequence` で fan-out する。`dx,dy,dz` は発生記録であり、クライアントはそれを積分して他人の絶対座標を捏造してはならない。他人の権威 pose は Snapshot の `player.*`、および非本人コピーに付ける明示的な `x= y= z=` として届く。`movementAck` / `lastProcessedInputSequence` を別の sequenced WorldUpdate として他人にだけ省略してはならない（欠番が Snapshot 再要求になる）。protocol version は 1 のまま。

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

Snapshot は公開プレイヤー pose を載せる:

```text
player.count=<n>;player.<i>.session=<id>;player.<i>.x=<f>;player.<i>.y=<f>;player.<i>.z=<f>
```

詳細と受入シナリオは `docs/specs/client-side-prediction-v1.md`。

## Server frame and client presentation contract

- The authoritative server advances the World at 20 Hz; one simulation frame is 50 ms.
- Commands received before a frame boundary are assigned a monotonic receive sequence and are processed in that order.
- Commands received while a frame is being processed are deferred to the next frame.
- A server update contains a `worldTick` and a monotonically increasing `sequence`.
- A Snapshot represents authoritative state. An Event represents an occurrence for presentation or audit; clients must not infer authoritative state from an Event alone.
- Clients may render at a different rate and interpolate Snapshot state. Client-side effect playback must not delay or overwrite authoritative state.
- Snapshot/Event identifiers are used to discard duplicates and detect gaps. A gap requests a new Snapshot rather than guessing missing state.

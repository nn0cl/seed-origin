# LISS-0128: クライアント通信シェル

- Status: in_progress
- Phase: phase-2-green
- Type: feature + client transport
- Priority: high
- Initial planning size: L
- Current planning size: M (narrow reconnect I/O slice; remainder stays L)
- Depends on: LISS-0121, LISS-0122, LISS-0044, **LISS-0147**, LISS-0154
- Related branch: `feature/liss-0152-client-side-prediction`
- Related ADR: `docs/architecture/adr/0020-game-client-mvp-godot.md`
  （MVP実装はGodot 4.7 + GDScriptで行う方針、2026-07-18時点で未承認）

## 目的

接続、仮ログイン、Command送信、WorldUpdate受信、Snapshot再要求、切断を扱うクライアント通信シェルを作る。

## 受入条件（Issue 全体）

- 接続状態とログイン状態を明示する。
- 部分送受信、送信FIFO、再接続、タイムアウトを安全に扱う。
- Snapshot要求中はEventをゲーム状態へ渡さない。
- UIや描画を通信コードへ混ぜない。
- 通信エラーを再現可能な状態機械として記録する。

## 本スライスで実装した範囲（2026-08-19）

RequestSnapshot が **再接続後に POSIX TCP ソケットへ載り**、ログイン済みなら
サーバーが既存の同一ティック最大 1 Snapshot を返せる最小 I/O。

- 既存 frame codec / OutboundFrameQueue / ClientWorldUpdateReceiver を使う。
- 接続状態（Disconnected / Connected / Failed）とログイン状態（Anonymous /
  LoggedIn）を分離する。
- 未ログインでは RequestSnapshot を書かない（サーバーも未ログインには
  WorldUpdate を出さない既存契約）。
- 再接続: `beginReconnect` → 新 TCP → Login → RequestSnapshot Command →
  Snapshot 適用。UDP・指数バックオフ・専用再送ループは作らない。

## 本スライスで残したもの

- タイムアウト、監視（LISS-0132）、本格再送アルゴリズム。
- プレイ UI / Godot・UE シェル（LISS-0129, LISS-0064）。
- 切断後に Login なしで同一 session を再開する政策（未仕様。現行
  SessionLifecycle はソケット切断で logout するため、本スライスは再 Login）。
- デプロイ、輻輳、運用フック。

## Ambiguity

- 再接続ウィンドウと registered-auth TTL の突合は ADR 0018 / LISS-0147 待ち。
- 本スライスは IPv4 アドレスへの blocking connect のあと non-blocking I/O。
  Happy Eyeballs / DNS は未仕様。
- ログイン応答と WorldUpdate が同一 TCP 読みに混在しうるため、先頭 2 バイトの
  WorldUpdate magic（0x5755）で demux する。これは既存フレーム形の読み取りであり
  新チャンネルではない。

## Gherkin（本スライス）

```gherkin
Scenario: Reconnect after login writes RequestSnapshot on the socket
  Given the client began reconnect and then received an accepted Login
  When the transport pumps
  Then it writes CommandType RequestSnapshot with empty payload
  And protocol version remains 1
  And the command session id is the accepted login session

Scenario: Unauthenticated reconnect does not write RequestSnapshot
  Given the client began reconnect without a login session
  When the transport pumps
  Then no RequestSnapshot frame is written

Scenario: Snapshot on the socket resumes after reconnect
  Given the client requested a Snapshot after reconnect
  When a Snapshot frame arrives on the same TCP stream after skipped Events
  Then snapshotRequested is false
  And expected sequence is snapshot.sequence + 1

Scenario: Loopback reconnect yields a server Snapshot
  Given a POSIX TCP server runtime is listening
  And the client reconnects, logs in, and sends RequestSnapshot
  When the server admits the request for the active session
  Then the client applies a Snapshot and stops requesting
```

## AI Planning Records

### AIP-0128-001

- Status: accepted (Adjudicator 2026-08-19: continue after LISS-0154; do not
  swallow full transport/ops scope)
- Created by:
  - Agent/environment: Cursor Auto / Composer
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-19
- Planning size: M
- Intended execution route: Feature Path AT-TDD on the CSP branch.
- Intended scope: reconnect TCP I/O so RequestSnapshot rides the existing
  connection/enqueue path.
- Estimated token range: 20k–50k
- Estimated token midpoint: 35k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: existing Connection/ClientSession, demux, CTest loopback.
- Assumptions: protocol v1; POSIX TCP only; re-login after disconnect.
- Confidence: high
- Revises:
- Revision reason:
- Superseded by:

## English

Build a client transport shell for connect, anonymous login, commands, WorldUpdates, snapshot requests, and disconnect. This slice wires RequestSnapshot onto POSIX TCP after reconnect. Timeout, backoff, UI, and auth remain follow-up work.

# LISS-0128: クライアント通信シェル

- Status: in_progress
- Phase: phase-1-red
- Related branch: `feature/liss-0128-reconnect-timeout-ops`
- Type: feature + client transport
- Priority: high
- Initial planning size: L
- Current planning size: M (narrow reconnect I/O slice; remainder stays L)
- Depends on: LISS-0121, LISS-0122, LISS-0044, **LISS-0147**, LISS-0154
- Related branch: `feature/liss-0128-reconnect-timeout-ops`
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

- **本 Phase 1 Red 対象（timeout / ops 最小）**
  - Snapshot 待ち・Login 応答待ちの monotonic 期限と `TransportErrorReason`
    記録。
  - 切断・プロトコル・タイムアウトを再現可能な状態機械として参照可能にする
    （`lastError` / `lastErrorDetail`）。
  - 運用向け最小カウンタ（`reconnectCount`, `snapshotRequestCount`）。
    詳細ログ・backpressure・個人情報排除は LISS-0132。
- **後続（本 issue 残だが Phase 1 外）**
  - 本格再送・指数バックオフ・UDP。
  - プレイ UI / Godot・UE シェル（LISS-0129, LISS-0064）。
  - 切断後に Login なしで同一 session を再開する政策（未仕様。現行
    SessionLifecycle はソケット切断で logout するため、現行は再 Login）。
  - デプロイ、輻輳、運用フック。

## Ambiguity

- 再接続ウィンドウと registered-auth TTL の突合は ADR 0018 / LISS-0147 待ち。
- 本スライスは IPv4 アドレスへの blocking connect のあと non-blocking I/O。
  Happy Eyeballs / DNS は未仕様。
- ログイン応答と WorldUpdate が同一 TCP 読みに混在しうるため、先頭 2 バイトの
  WorldUpdate magic（0x5755）で demux する。これは既存フレーム形の読み取りであり
  新チャンネルではない。

## Gherkin（timeout / ops スライス — Phase 1 Red 2026-08-20）

```gherkin
Scenario: Snapshot wait timeout records failure and keeps snapshot pending
  Given the client reconnected and received an accepted Login
  And snapshot wait timeout is 5000 ms
  When 5001 ms elapse without a Snapshot
  And the transport pumps with the injected monotonic clock
  Then linkState is Failed
  And lastError is SnapshotWaitTimeout
  And snapshotRequested is true
  And snapshotRequestCount is 1

Scenario: Login response wait timeout records failure during reconnect
  Given the client began reconnect on an open socket
  And login response wait timeout is 3000 ms
  When 3001 ms elapse without a Login response
  And the transport pumps with the injected monotonic clock
  Then linkState is Failed
  And lastError is LoginResponseTimeout
  And authState is Anonymous

Scenario: Peer close records PeerClosed on a logged-in transport
  Given a logged-in transport on an open socket
  When the peer closes the connection
  And the transport pumps
  Then linkState is Disconnected
  And lastError is PeerClosed
  And authState is Anonymous

Scenario: Corrupt inbound frame records ProtocolError
  Given a connected transport
  When corrupt bytes arrive on the socket
  And the transport pumps
  Then linkState is Failed
  And lastError is ProtocolError

Scenario: Successful reconnect increments reconnect counter
  Given a logged-in transport that begins reconnect
  When reconnect completes with login and snapshot applied
  Then reconnectCount is 1
  And lastError is None
```

## Gherkin（reconnect I/O スライス — 2026-08-19 完了）

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

### AIP-0128-002

- Status: accepted (Adjudicator 2026-08-20: Phase 1 Red for timeout/ops slice)
- Created by:
  - Agent/environment: Cursor Auto
  - Model as displayed: Composer
  - Reasoning setting as displayed: N/A
  - N/A reason: Cursor agent display does not expose a separate reasoning slider
- Created at: 2026-08-20
- Planning size: M
- Intended execution route: Feature Path AT-TDD Phase 1 Red on
  `feature/liss-0128-reconnect-timeout-ops`.
- Intended scope: monotonic timeout deadlines, transport error state machine,
  minimal ops counters (`reconnectCount`, `snapshotRequestCount`).
- Estimated token range: 15k–40k
- Estimated token midpoint: 25k
- Token metric: combined prompt+completion for one execution attempt
- Estimation basis: existing ClientTransportShell, socketpair tests, ClockPort.
- Assumptions: protocol v1; injectable monotonic clock; re-login after
  disconnect unchanged; connect remains blocking (connect timeout deferred).
- Confidence: high
- Revises: AIP-0128-001 (narrow I/O slice complete on main)
- Revision reason: remainder scope split to dedicated branch after LISS-0152 merge
- Superseded by:

## English

Build a client transport shell for connect, anonymous login, commands, WorldUpdates, snapshot requests, and disconnect. This slice wires RequestSnapshot onto POSIX TCP after reconnect. Timeout, backoff, UI, and auth remain follow-up work.

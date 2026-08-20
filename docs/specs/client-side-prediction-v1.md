# Feature: Local player client-side prediction and server reconciliation

## EARS

When a local player issues a Move command with a per-player monotonically
increasing `clientInputSequence`, the client shall apply that input to local
prediction state immediately and send only the input (not position) to the
server.

When the server accepts a sequenced Move, it shall apply the input on the
fixed 20 Hz world tick, retain the last processed `clientInputSequence` for
that player, and return authoritative `x,y,z`, `worldTick`, and
`lastProcessedInputSequence` to that player's session only.

When the client receives an authoritative movement ack for its own session, it
shall restore prediction state to that authoritative pose, discard pending
inputs with sequence less than or equal to the ack, and replay remaining
unacked inputs. Acks for other sessions shall not drive local prediction.

If a Move repeats a `clientInputSequence` already accepted or processed for
that player, the server shall ignore it and shall not apply it twice.

If a Move carries a `clientInputSequence` older than the last accepted value,
or farther ahead than the admission window, the server shall ignore or reject
it without mutating world state.

While rendering, the client shall keep prediction (logical) state distinct
from render state. Small pose error shall be smoothed over 100–200 ms. Large
error or a snapshot/teleport baseline shall snap.

When an operator assigns a new unique PlayerName to an auth PlayerId, the
previous trimmed name shall be released from the claimed-name registry and
the new trimmed name shall be exclusive to that auth PlayerId. Player-path
rename shall remain rejected. Empty or whitespace-only names shall remain
rejected. Auth PlayerId and gameplay id shall not change.

## Gherkin

```gherkin
Scenario: Server acks a monotonic client input sequence
  Given a player is present in the field at the origin
  And the player sends Move payloads 1:1,0,0 then 2:1,0,0
  When the server applies those inputs on a world frame
  Then the player's authoritative position is (2,0,0)
  And the movement ack lastProcessedInputSequence is 2

Scenario: Duplicate client input sequence is not applied twice
  Given a player is present in the field at the origin
  And the player sends Move payload 1:1,0,0 twice
  When the server applies queued inputs on a world frame
  Then the player's authoritative position is (1,0,0)
  And lastProcessedInputSequence is 1

Scenario: Out-of-order or extremely old input is ignored
  Given a player is present in the field at the origin
  And the player sends Move payload 2:1,0,0 then 1:1,0,0
  When the server applies queued inputs
  Then the player's authoritative position is (1,0,0)
  And the stale sequence 1 is not applied

Scenario: Client reconverges with authoritative state plus unacked replay
  Given the client predicted inputs 1,2,3 from the origin
  And the server ack reports pose (1,0,0) with lastProcessedInputSequence 1
  When the client reconciles
  Then pending input 1 is discarded
  And inputs 2 and 3 are replayed
  And predicted position is (3,0,0)

Scenario: World update sequence gap is not treated as an input ack gap
  Given the client expected WorldUpdate sequence 1
  When an Event with WorldUpdate sequence 3 arrives
  Then the receiver requests a snapshot
  And pending predicted inputs are not acknowledged

Scenario: Snapshot rebases pending inputs
  Given the client has unacked inputs 4 and 5
  When a Snapshot reports local pose (10,0,0) and lastProcessedInputSequence 3
  Then inputs at or below 3 are discarded
  And inputs 4 and 5 are replayed on (10,0,0)

Scenario: Delayed and lossy acks still reconverge
  Given the client predicted five 1-unit +X moves
  When acks are delayed by 80 ms, 150 ms, or 250 ms of 20 Hz frames
  Or an intermediate ack is dropped and a later ack arrives
  Then predicted position matches the authoritative pose plus remaining unacked inputs

Scenario: Movement ack is owner-session only
  Given two logged-in sessions A and B
  When A moves and the server publishes the world frame
  Then both sessions receive A's public movement= Event with the same WorldUpdate.sequence
  And A's copy includes lastProcessedInputSequence and authoritative pose
  And B's copy has no lastProcessedInputSequence and may carry public x= y= z=
  And B does not request a Snapshot

Scenario: Foreign movement ack does not drive local prediction
  Given the local session is 10 with a pending predicted input
  When a movement ack for session 99 arrives
  Then the local predictor is unchanged
  And the WorldUpdate sequence still advances

Scenario: Foreign public pose is applied without owner ack
  Given two logged-in sessions A and B
  And B has applied a Snapshot listing A as gameplay id 7 session 10
  And B has a pending local prediction
  When A moves and B receives B's copy of the public movement Event
  Then B's copy includes explicit public x,y,z for A
  And B's copy does not include lastProcessedInputSequence
  And B applies A's pose to the remote store keyed by gameplay id 7
  And B's local predictor is unchanged

Scenario: Snapshot rebases remote public poses
  Given the client has remote gameplay id 7 at (1,0,0)
  When a Snapshot reports player session 99 id 7 at (10,0,0) without local ack fields
  Then remote gameplay id 7 is rebased to (10,0,0)
  And the local predictor is unchanged

Scenario: Delta-only movement does not invent remote coordinates
  Given the client has no remote pose for gameplay id 7
  When a public movement= Event for session 99 arrives with dx,dy,dz and no x,y,z
  Then no remote pose is created for session 99 or gameplay id 7

Scenario: Full Snapshot replaces every public pose including idle players
  Given the client has remote gameplay id 7 at (1,0,0)
  When a Snapshot reports session 99 id 7 at (1,0,0) and session 11 id 11 at (5,0,0)
  Then remote 7 is replaced with (1,0,0)
  And remote 11 is placed at (5,0,0)

Scenario: Reconnect Snapshot keeps one remote for the same gameplay id
  Given the client has remote gameplay id 7 bound to session 99 named Hero
  When a Snapshot lists session 100, id 7, name Hero
  Then the remote store has one pose
  And that pose is keyed by gameplay id 7
  And its session field is 100
  And the HUD display field is Hero

Scenario: Public Snapshot auth PlayerId is not applied
  Given a Snapshot payload includes player.0.authPlayerId
  When the client applies it
  Then the snapshot is rejected
  And no remote pose is stored

Scenario: Delta Event omits stationary public poses
  Given a Snapshot placed session 99 at (1,0,0)
  When only the local player moves on the next 20 Hz tick
  Then the Event carries public pose for the mover
  And the Event does not re-send session 99
  And session 99 remains at (1,0,0) on the receiver

Scenario: Join Snapshot includes the logging-in session at the early spawn
  Given the Field has no players
  When session 21 is placed after Login
  And the login-success tick captures a join Snapshot
  Then the Snapshot payload contains player.count=1
  And the Snapshot lists session 21 at (0,0,0)
  And the Snapshot lists a gameplay id distinct from session 21
  And the Snapshot does not list the auth PlayerId

Scenario: Duplicate PlayerName is rejected
  Given session 21 is already placed with PlayerName Hero
  When session 22 is placed with the same PlayerName Hero
  Then placement fails
  And session 21 remains on the Field

Scenario: Empty PlayerName placement is rejected
  Given no operator PlayerName is assigned for the authenticating player
  When session 21 is placed after Login
  Then placement fails

Scenario: Whitespace-only PlayerName placement is rejected
  Given an operator PlayerName that is only padding characters
  When session 21 is placed after Login
  Then placement fails
  And trim of that name is treated as empty

Scenario: Claimed PlayerName survives logout and Field unset
  Given session 21 is placed with PlayerName Hero
  And the session logs out
  And the Field entity is unset
  When session 22 is placed with PlayerName Hero for a different auth PlayerId
  Then placement fails

Scenario: Trimmed PlayerName collides with an existing claim
  Given session 21 is placed with PlayerName Hero
  When session 22 is placed with PlayerName " Hero " for a different auth PlayerId
  Then placement fails
  And session 21 remains named Hero

Scenario: Snapshot requires a non-empty PlayerName
  Given a Snapshot player entry with an omitted or empty name
  When the client applies it
  Then the snapshot is rejected
  And no remote pose is stored

Scenario: Player cannot rename
  Given a placed player named Hero
  When the player path requests a name change
  Then the request fails
  And the name remains Hero
  When an operator assigns Mage
  Then the name is Mage

Scenario: Operator rename releases the previous claimed name
  Given session 21 is placed with PlayerName Hero
  When an operator assigns Mage to the same auth PlayerId
  Then the name is Mage
  And gameplay id and auth PlayerId are unchanged
  And a player-path rename still fails
  When session 22 is placed with PlayerName Hero for a different auth PlayerId
  Then placement succeeds
  And Mage remains exclusive to the original auth PlayerId

Scenario: Operator rename after logout still releases the previous claim
  Given session 21 was placed with PlayerName Hero
  And the session logs out
  And the Field entity is unset
  When an operator assigns Mage to that auth PlayerId
  Then a different auth PlayerId may be placed as Hero
  And Mage remains exclusive

Scenario: Failed operator rename keeps the previous claim
  Given session 21 is named Hero and another auth already claims Mage
  When an operator assigns Mage to session 21's auth PlayerId
  Then the assignment fails
  And session 21 remains named Hero
  And Hero stays claimed

Scenario: Reconnect keeps gameplay id and auth PlayerId
  Given a placed player with auth PlayerId 9001 and gameplay id G
  When a new session logs in for the same auth PlayerId
  Then gameplay id is G
  And auth PlayerId is 9001
  And the session id is the new session

Scenario: Join Snapshot includes idle others already on the Field
  Given session 99 is already on the Field at (6,0,0)
  When session 21 is placed after Login
  And the login-success tick captures a join Snapshot
  Then player.count=2
  And the Snapshot lists session 99 at (6,0,0)
  And the Snapshot lists session 21 at (0,0,0)

Scenario: Login placement does not emit a movement Event
  Given session 21 was placed after Login
  When a world frame applies with no movement inputs
  Then no movement= Event is published

Scenario: Sequence gap builds a RequestSnapshot Command
  Given the client expected WorldUpdate sequence 1
  And the local session id is 21
  When an Event with WorldUpdate sequence 5 arrives
  Then the receiver requests a snapshot
  And the client builds CommandType RequestSnapshot with empty payload
  And protocol version remains 1
  And the command session id is 21

Scenario: RequestSnapshot fans out one Snapshot on the shared sequence
  Given two authenticated sessions send RequestSnapshot in the same tick
  When the tick captures snapshots for pending requests
  Then exactly one Snapshot is appended on the global WorldUpdate.sequence
  And public player.* matches the join Snapshot form
  And owner copies may add local.* without changing sequence
  And no movement= Event is published solely because of the request

Scenario: Snapshot after RequestSnapshot resumes Events
  Given the client requested a Snapshot after reconnect
  When a Snapshot is applied after skipped Events
  Then snapshotRequested is false
  And expected sequence is snapshot.sequence + 1
  And a later Event at that sequence is applied
```

## External Dependencies

- None. Tests use in-memory Field, queues, and client receivers. No sockets.

## Out of Scope

- Remote player or NPC prediction.
- Predicting damage, death, MP, chat, or inventory.
- Client renderer / UI framework (LISS-0064).
- Wire serialization format replacement.
- Per-connection WorldUpdate.sequence (global monotonic sequence is unchanged).
- Auth work in LISS-0146–0150 (this worktree consumes a PlayerId port stub).
- Reconnect socket I/O remainder (timeout, ops, UI) stays LISS-0128.
  The 2026-08-19 slice sends RequestSnapshot on POSIX TCP after
  beginReconnect and Login. RequestSnapshot wire Command is LISS-0154.
- Unicode or case-fold normalization of PlayerName.
- Account-wide name ledger inside seed-auth (early uniqueness is an
  in-memory claimed-name registry on the world server).

## Ambiguities

- None remaining for the full vs delta WorldUpdate contract in this issue.
  Adjudicator (2026-08-17): movement ack is owner-session only; protocol
  version stays 1; public movement stays the existing `movement=` Event
  broadcast. Adjudicator (2026-08-18): remote interpolation is the
  CSP-minimum slice of LISS-0082; public player pose belongs on Snapshot
  and as explicit `x= y= z=` on non-owner copies; do not invent coordinates
  by integrating `dx,dy,dz`. Adjudicator (2026-08-19): full fetch and delta
  fetch share the same WorldUpdate sequence column (not a second channel);
  Snapshot is the full replace (join, sequence gap, reconnect), including
  stationary public poses; 20 Hz Event is the delta (movers only);
  `lastProcessedInputSequence` stays on the owner's copy of that sequence;
  20 Hz is the tick and delta Event period, not a full-Snapshot rate.
  RequestSnapshot is command type 7 with empty payload (LISS-0154).
  Login Field placement follows LISS-0153 (configurable spawn, four identity
  roles, unique non-empty PlayerName assigned by operators only). Observers key
  remotes by gameplay id (`player.<i>.id`); session is communication; HUD uses
  PlayerName. Auth PlayerId and gameplay id do not change on reconnect;
  session does. Login claimedId is not a display name. Whitespace-only
  names are empty after ASCII trim. Uniqueness is trim-then-exact-match
  and survives logout and Field unset via an in-memory claimed-name
  registry. An operator rename releases the previous claim and monopolizes
  the new name. Unicode and case-fold normalization stay out of scope.

## Move payload

- Sequenced: `clientInputSequence:dx,dy,dz` (example `3:1,0,0`).
- Legacy: `dx,dy,dz` (example `1,2,3`) maps to `clientInputSequence` 0 and is
  not admitted into prediction ack tracking.

`clientInputSequence` is owned by the client, starts at 1, and is independent
of `MovementIntent.sequence` and `WorldUpdate.sequence`.

## Full vs delta on one WorldUpdate sequence

Join, sequence gap, and reconnect use the same global
`WorldUpdate.sequence` column as the 20 Hz movement Event. Do not split
full fetch and delta fetch onto another channel or another sequence
space. Protocol version stays 1.

**Full (Snapshot):** replace every present player's public pose, including
players who did not move. Emit on join, when a sequence number is missing,
and on reconnect. A gap is never filled by guessing from Events; the
receiver takes another full fetch.

**Delta (Event):** the 20 Hz tick period. Only movers appear as a public
`movement=` Event. Non-owner copies of those Events may carry the
post-tick authoritative pose as `;x=;y=;z=`. Stationary players are not
resent on that tick.

Owner ack (`lastProcessedInputSequence` and colon-delimited pose fields)
is attached only to the owner's copy of that same sequence. The 20 Hz
period is the simulation tick and the delta Event cadence. The server
does not emit a full Snapshot every 20 Hz tick.

The client asks for another full fetch with `RequestSnapshot` (type 7,
empty payload, protocol version 1). The session id is the connection's
internal id. The server coalesces requests and join needs into at most
one Snapshot on that tick, on the same WorldUpdate.sequence column.

## Login Field placement (LISS-0153)

Login places a Field entity and binds the connection session to it.

- **Auth PlayerId**: opaque, from `AuthenticatedPlayerIdPort` (early stub).
  Persistent across reconnect. Not on Snapshot, movement, or UI.
- **session ID**: `session.internalId`. Connection header and Snapshot
  `player.<i>.session`. Changes on reconnect. Not shown in game UI.
- **Gameplay id**: Field `Player::getPlayerId()`, Attack/CastSpell `targetId`.
  Snapshot `player.<i>.id`. World-allocated, not the auth PK. Not shown in UI.
- **PlayerName**: display only (`player.<i>.name`). Leading and trailing
  ASCII padding (`space`, tab, CR, LF, VT, FF) is trimmed, then the name
  must be non-empty. Uniqueness is exact match of that trimmed value
  (case-preserving; no Unicode fold). Login `claimedId` is not a name.
  Operators assign names (spawn settings, test stub, or
  `operatorSetPlayerName`). No player rename command.
  Uniqueness is a claimed-name registry, not Field occupancy: logout or
  `unsetPlayer` does not release the name. Operator rename of the same
  auth PlayerId does: the previous trimmed name is unclaimed and the new
  trimmed name is exclusive. Players still cannot rename themselves.
  Early storage is in-memory on the world server. Auth persistence is
  LISS-0146–0150.

Early spawn defaults, when settings are unset: pose `(0,0,0)`, HP/MP `10,10`.
Spawn HP/MP clamp to settings max (early `1024,1024`). `Status::gainHp` still
saturates at `long` max (existing overflow cap; Adjudicator proposed 1024 as
the spawn/game max).

Reconnect binds the new session to the existing auth PlayerId entity (pose and
Status stay). Disconnect unbinds the session and omits the entity from public
poses until rebound; it does not `unsetPlayer`. The server replies with a
Disconnect ack (Command type 6, empty payload on success). The client resets
transport auth only after that Accepted ack, then closes TCP. A Rejected ack
leaves the client LoggedIn and leaves TCP open. The server flushes the
Accepted ack before closing the connection. Reconnect uses a new TCP
(`beginReconnect` / `connectTcp`).

Observers track other people by Snapshot `player.<i>.id` (gameplay id). The
remote pose store uses that id as the key. `player.<i>.session` is communication
only (movement Event routing). HUD/display reads `player.<i>.name` only.
Rebinding a new session to the same gameplay id updates the existing remote
entity; it does not duplicate it. Name may be refreshed as a display field.
Auth PlayerId is never parsed from public Snapshot fields. Movement Events
still carry `session:` and only update a remote that a Snapshot already keyed.

The login-success tick still emits at most one join Snapshot (not a 20 Hz
full Snapshot). Placement does not invent a `movement=` Event.

## Public movement vs owner ack

Logged-in sessions receive the same sequenced public Event:

```text
movement=session:<id>;dx=<f>;dy=<f>;dz=<f>;clientInputSequence=<u64>;inputSequence=<u64>
```

The `dx,dy,dz` fields remain an occurrence record. Clients must not integrate
those deltas into another player's absolute coordinates.

Other players' authoritative pose is explicit public state:

- Snapshot `player.<i>.session` / `player.<i>.x,y,z` for every present player
  (full replace, including idle).
- The non-owner copy of a sequenced `movement=` Event may also carry public
  `x=<f>;y=<f>;z=<f>` copied from the same post-tick pose used for the owner
  ack. That copy still uses the same `WorldUpdate.sequence`.

`lastProcessedInputSequence` and colon-delimited owner ack fields (`x:`) stay
on the owner's copy only. Snapshot `local.*` rebases the local predictor and
must not be mixed into `player.*`. Remote entities are not predicted; the
client applies public pose and interpolates small render error (100–200 ms) or
snaps large error / snapshot rebase.

Authoritative pose and `lastProcessedInputSequence` for reconciliation are
attached only to the **owner's copy** of that same `WorldUpdate.sequence`
(payload differs; sequence and eventId do not). Other sessions must not
receive those owner-ack fields.

Do not emit owner ack as a separate sequenced WorldUpdate that others omit:
clients treat a sequence gap as a Snapshot request.

Standalone `movementAck=` Events remain parseable for tests and defense in
depth. Delivery must not use them as an extra global sequence.

## Owner-only ack fields

Attached to the owner's copy of the public movement Event (same sequence):

```text
;x:<f>;y:<f>;z:<f>;worldTick:<u64>;lastProcessedInputSequence:<u64>
```

Standalone Event payload (must not be a distinct global sequence that others
skip):

```text
movementAck=session:<id>;x:<f>;y:<f>;z:<f>;worldTick:<u64>;lastProcessedInputSequence:<u64>
```

The receiving client applies ack data to `LocalMovementPredictor` only when
`sessionId` matches the bound local session.

Optional Snapshot fields for local predictor rebase (owner-oriented, not a
public remote pose):

```text
local.x=<f>;local.y=<f>;local.z=<f>;local.lastProcessedInputSequence=<u64>
```

Public Snapshot player poses (every present player, including idle; no input ack):

```text
player.count=<n>;player.<i>.session=<id>;player.<i>.x=<f>;player.<i>.y=<f>;player.<i>.z=<f>;player.<i>.id=<gameplayId>;player.<i>.name=<name>
```

`name` is required and must be non-empty after ASCII trim. `id` is omitted
only for pose fixtures that never assigned a gameplay id. Auth PlayerId is
never present.

Non-owner copies of `movement=` may append public pose without owner ack:

```text
;x=<f>;y=<f>;z=<f>
```

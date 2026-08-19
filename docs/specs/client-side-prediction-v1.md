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
  And B has a pending local prediction
  When A moves and B receives B's copy of the public movement Event
  Then B's copy includes explicit public x,y,z for A
  And B's copy does not include lastProcessedInputSequence
  And B applies A's pose to the remote store
  And B's local predictor is unchanged

Scenario: Snapshot rebases remote public poses
  Given the client has remote session 99 at (1,0,0)
  When a Snapshot reports player session 99 at (10,0,0) without local ack fields
  Then remote 99 is rebased to (10,0,0)
  And the local predictor is unchanged

Scenario: Delta-only movement does not invent remote coordinates
  Given the client has no remote pose for session 99
  When a public movement= Event for 99 arrives with dx,dy,dz and no x,y,z
  Then no remote pose is created for session 99

Scenario: Full Snapshot replaces every public pose including idle players
  Given the client has remote session 99 at (1,0,0)
  When a Snapshot reports session 99 at (1,0,0) and session 11 at (5,0,0)
  Then remote 99 is replaced with (1,0,0)
  And remote 11 is placed at (5,0,0)

Scenario: Delta Event omits stationary public poses
  Given a Snapshot placed session 99 at (1,0,0)
  When only the local player moves on the next 20 Hz tick
  Then the Event carries public pose for the mover
  And the Event does not re-send session 99
  And session 99 remains at (1,0,0) on the receiver

Scenario: Join Snapshot includes the logging-in session at the temporary origin
  Given the Field has no players
  When session 21 is placed after Login
  And the login-success tick captures a join Snapshot
  Then the Snapshot payload contains player.count=1
  And the Snapshot lists session 21 at (0,0,0)

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
```

## External Dependencies

- None. Tests use in-memory Field, queues, and client receivers. No sockets.

## Out of Scope

- Remote player or NPC prediction.
- Predicting damage, death, MP, chat, or inventory.
- Client renderer / UI framework (LISS-0064).
- Wire serialization format replacement.
- Per-connection WorldUpdate.sequence (global monotonic sequence is unchanged).
- Auth work in LISS-0146–0150.
- RequestSnapshot as a wire Command (follow-up Issue; LISS-0122 already
  leaves the actual request Command to later work).
- Durable spawn policy (initial zone, HP/MP, PlayerId vs sessionId split,
  reconnect restore). Temporary Login placement is specified below and
  replaced by LISS-0153.

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
  RequestSnapshot remains a follow-up Issue. Post-Login Field placement
  uses the temporary origin convention below until LISS-0153.

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

How the client asks for a Snapshot on the wire (`RequestSnapshot`
Command) remains a follow-up Issue. This document does not define that
payload.

## Temporary Login Field placement (until LISS-0153)

Login currently binds `session.internalId` without putting a Player on
the Field, so join Snapshots were empty (`player.count=0`). Until LISS-0153
defines durable spawn, Login success places the session with the existing
test convention:

- `PlayerId` equals `session.internalId` (Snapshot `player.<i>.session`
  already uses `Player::getPlayerId()`).
- Pose is origin `(0, 0, 0)` via `Position(sessionId, 0, 0, 0)`, matching
  idle-observer tests.
- `Status()` is the default constructor (HP 0, MP 0), matching existing
  Snapshot Field tests. This is not a combat spawn table.
- Placement uses `Field::setPlayer`. Logout/disconnect uses
  `Field::unsetPlayer` so departed sessions leave the public pose list.
- This is a temporary initial pose. LISS-0153 replaces it.

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
player.count=<n>;player.<i>.session=<id>;player.<i>.x=<f>;player.<i>.y=<f>;player.<i>.z=<f>
```

Non-owner copies of `movement=` may append public pose without owner ack:

```text
;x=<f>;y=<f>;z=<f>
```

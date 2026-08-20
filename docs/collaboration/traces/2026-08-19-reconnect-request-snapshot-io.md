# Trace: Reconnect socket I/O for RequestSnapshot (LISS-0128 slice)

- Date: 2026-08-19
- Path: Feature Path
- Issue: LISS-0128 (depends on LISS-0154)
- Planning record: AIP-0128-001
- Branch: `feature/liss-0152-client-side-prediction`
- Phase: Phase 2 Green for the narrowed I/O slice (Adjudicator authorized
  minimum Green in the same continue request)

## Request

Wire RequestSnapshot onto POSIX TCP after reconnect so the server can return
one Snapshot. Do not touch auth. Do not push. Protocol v1. No UDP, no 20 Hz
full Snapshot, no LISS-0153 spawn.

## Included

- Client inbound demux of LoginResponse vs WorldUpdate on one TCP stream
- ClientTransportShell: connect, attach, beginReconnect, login enqueue,
  FIFO flush, pump
- Loopback + socketpair tests mapping the slice Gherkin
- Listener boundPort for ephemeral-port tests

## Omitted

- seed-auth / LISS-0146–0150
- Timeout, exponential backoff, dedicated retransmission
- LISS-0132 observability, LISS-0129 play UI, LISS-0153 spawn
- Combat `queues_attack_and_spell_intents` (pre-existing; not this slice)

## Routing

Composer locally; CMake/CTest for verification. No LLM output in game state.

## Verification

- Full `seed_tests`: new demux/transport tests pass on repeated runs.
  Suite still stops at existing CombatCommandHandlerTest
  `queues_attack_and_spell_intents` (spell handle not accepted).
- Peer TCP close is treated as Closed (ECONNRESET/EPIPE), not a hard
  frame failure, so reconnect can accept the next socket.

## Next safe action

Keep LISS-0128 remainder (timeout/ops/UI) separate. LISS-0153 remains the
CSP spawn follow-up. Do not merge/push without Adjudicator confirmation.

# Trace: Close TCP after Accepted Disconnect ack (LISS-0152)

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0152
- Branch: `feature/liss-0152-client-side-prediction`
- Phase: AT-TDD Red then Green in one Adjudicator-requested slice

## Request

After the server Accepts Disconnect and the client processes that ack,
explicitly close TCP. Rejected acks leave LoggedIn and keep the socket.
Reconnect uses a new TCP (`beginReconnect` / `connectTcp`).

## Included

- Client `ClientTransportShell::applyDisconnectResponse` calls `beginReconnect`
  (existing `close` + inbound/auth reset) on Accepted ack
- Server `ClientSession::requestCloseAfterFlush` after Accepted ack enqueue;
  `flushOutbound` drains then `shutdown`+`close`
- Acceptance tests: socketpair EOF after Accepted; loopback re-login on a
  new `connectTcp`

## Omitted

- seed-auth, LISS-0155, PlayerName case, zone spawn
- Disconnect ack wire change

## Adjudicator decisions applied

- Order: flush Accepted ack, then close. Both ends may close (idempotent).
- Rejected: no TCP close.
- Protocol version stays 1.

## Verification

Full `seed_tests` in `seed-origin-prediction/build`.

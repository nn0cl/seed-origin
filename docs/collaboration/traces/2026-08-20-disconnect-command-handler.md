# Trace: Disconnect command ends session and omits public pose (LISS-0152)

- Date: 2026-08-20
- Path: Feature Path
- Issue: LISS-0152
- Branch: `feature/liss-0152-client-side-prediction`
- Phase: AT-TDD Red then Green in one Adjudicator-requested slice

## Request

Implement protocol v1 `Disconnect` so it ends the logged-in session,
unbinds the Field session (public pose omitted), keeps claimed PlayerName
and gameplay/auth ids for reconnect, and retargets the unimplemented-command
test now that Move is handled.

## Included

- `DisconnectCommandHandler`, dispatcher routing, rate limit (2/frame)
- `FieldSessionPresence::removeAfterLogout` (existing)
- `SessionRegistry::logout`
- `ServerRuntime::processClientFrames` lifecycle unbind on accepted Disconnect
- acceptance tests (handler, dispatcher, runtime loopback)

## Omitted

- seed-auth, LISS-0155, zone/logout spawn

## Adjudicator decisions applied

- Logout/Field unset does not release claimed PlayerName (LISS-0153 / CSP spec).
- Reconnect rebinds same auth PlayerId entity (existing presence tests).
- Inactive Disconnect rejects with explicit error (no silent no-op).
- Client transport resets `auth` to Anonymous after an Accepted Disconnect ack; TCP stays open.
- A Rejected Disconnect ack leaves the client LoggedIn.
- Disconnect and RequestSnapshot reject non-empty payload (`validateCommand`).

## Verification

Full `seed_tests` in `seed-origin-prediction/build`.

## Open decisions

- Should Disconnect explicitly close the TCP connection after the ack, or remain
  session-only end with the socket open (current: open, matching re-login)?

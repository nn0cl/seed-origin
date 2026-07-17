# ADR 0019: Admin Backend Implementation Language is Kotlin + Spring Boot

## Status

Proposed — awaiting Adjudicator review. **No implementation may begin until
this ADR is explicitly reviewed and approved.** ADR 0017 was previously
implemented before review, which was a process violation
(`AGENTS.md` Prime Directive: "No implementation without a reviewed
acceptance specification"); this ADR corrects that by stopping at the
proposal stage.

## Context

ADR 0017 decided the admin HTTP surface's data model, security policy, and
also — without a separate review checkpoint — its implementation language
and library (C++ / `cpp-httplib`). The Adjudicator has since decided the
admin backend should be implemented in **Kotlin with Spring Boot**, with the
existing React front-end decision (ADR 0017 decision 5 / LISS-0145)
unchanged.

This ADR supersedes ADR 0017's language/library choice only. The
data-model and security-policy decisions already made in ADR 0017 are
language-independent and are **not** being reopened by this ADR:

- `admin_users` table shape (`id`, `username` unique, `password_hash`).
- Password hashing via PostgreSQL `pgcrypto` (`crypt()`/bcrypt) — carries
  over regardless of which language queries it.
- Session TTL: 1 hour fixed, no idle timeout.
- Login lockout: 3 consecutive failures locks for 15 minutes.
- No HTTP "create admin" endpoint; bootstrap remains a manual DB operation
  (still open for reconsideration — see Open Questions).

## Decision (proposed, not yet approved)

1. The admin backend becomes a Kotlin + Spring Boot service, replacing the
   C++ `seed_admin` executable (`src/AdminMain.cpp`,
   `include/seed/AdminAuthStore.h`/`src/AdminAuthStore.cpp`,
   `include/seed/AdminSessionStore.h`/`src/AdminSessionStore.cpp`,
   `include/seed/AdminLoginLockout.h`/`src/AdminLoginLockout.cpp`, and the
   `seed_admin`/`httplib`-related `CMakeLists.txt` entries).
2. It connects to the same PostgreSQL database (`seed_identity`) and the
   same `admin_users`/`identity_aliases` tables, via Spring Data JDBC or
   Spring Data JPA (exact choice not decided here — open question).
3. It remains a separate process/deployment from `seed_server` (C++) and
   any future `seed_auth` service, consistent with the Adjudicator's
   earlier instruction that these stay separate instances.
4. Session/token behavior (Bearer-only, no cookies, 1-hour TTL, 3-attempt
   lockout) is preserved as a behavioral contract, reimplemented in Kotlin
   rather than ported line-by-line from the C++ version.
5. Docker Compose (`db/docker-compose.yml`) gains a service definition for
   the Kotlin/Spring Boot admin backend and the React front-end, per the
   Adjudicator's stated preference for Docker Compose deployment of the
   Kotlin/React pieces.

## Open questions (must not guess — require explicit Adjudicator answers)

- Spring Data JDBC vs. JPA/Hibernate for the `admin_users`/`identity_aliases`
  access layer.
- Build tool: Gradle (Kotlin DSL) vs. Maven.
- Whether the existing C++ `seed_admin` code is deleted immediately or kept
  until the Kotlin replacement is verified working (this question was asked
  once already and not yet answered).
- Whether `pgcrypto` password verification stays server-side in SQL (as the
  C++ version did, avoiding any password-hashing library in the new
  service too) or moves to a JVM library (e.g. Spring Security's
  `BCryptPasswordEncoder`) — these produce different hash formats and are
  **not interchangeable**; picking wrong would lock out the bootstrap admin
  account.
- Dependency-adoption checklist (`docs/architecture/dependency-policy.md`)
  for Spring Boot and its transitive dependencies has not been performed.

## Consequences (anticipated, pending approval)

Positive:

- Faster iteration on admin CRUD-style features going forward, per the
  Adjudicator's stated reasoning.
- Data/security policy already decided in ADR 0017 is reused unchanged,
  minimizing re-litigated ground.

Negative:

- The working, manually-verified C++ implementation (ADR 0017/LISS-0144) is
  discarded or left dormant.
- Introduces a second language runtime (JVM/Kotlin/Gradle or Maven) into a
  project that was previously C++-only, which is a meaningful new
  operational surface (build, deploy, monitor).
- `seed_auth` (LISS-0146, not yet built) will likely follow the same
  language choice for consistency, though that is not decided by this ADR.

## Related documents

- `docs/architecture/adr/0017-admin-authentication-and-http-surface.md`
  (superseded for implementation language only; data/security decisions
  remain in effect)
- `docs/issues/LISS-0144-admin-authentication-http-surface.md`
- `docs/issues/LISS-0145-admin-ui-react-spa.md`

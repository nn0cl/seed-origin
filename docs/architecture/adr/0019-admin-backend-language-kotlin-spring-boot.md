# ADR 0019: Admin Backend Implementation Language is Kotlin + Spring Boot

## Status

Proposed — awaiting final Adjudicator sign-off before implementation
begins. Most open questions have been answered (2026-07-18); one inference
below (password-hashing boundary for `seed_admin` itself) needs explicit
confirmation. ADR 0017 was previously implemented before review, which was
a process violation (`AGENTS.md` Prime Directive: "No implementation
without a reviewed acceptance specification"); this ADR corrects that by
stopping for review before any code is written.

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

## Decision

1. The admin backend becomes a Kotlin + Spring Boot service, replacing the
   C++ `seed_admin` executable (`src/AdminMain.cpp`,
   `include/seed/AdminAuthStore.h`/`src/AdminAuthStore.cpp`,
   `include/seed/AdminSessionStore.h`/`src/AdminSessionStore.cpp`,
   `include/seed/AdminLoginLockout.h`/`src/AdminLoginLockout.cpp`, and the
   `seed_admin`/`httplib`-related `CMakeLists.txt` entries). **The C++
   version is kept, unmodified, until the Kotlin replacement is built and
   verified working, then discarded** — not deleted immediately.
2. Build tool: **Gradle (Kotlin DSL)**.
3. Persistence: **MyBatis** as the SQL mapper, not Spring Data JPA/
   Hibernate. The persistence layer follows **DDD + Clean Architecture**
   layering: MyBatis mapper interfaces and generated SQL live in the
   infrastructure/adapter layer only; domain and application-service code
   depend on repository interfaces, never on MyBatis types directly. This
   mirrors the same dependency-direction rule `AGENTS.md` already applies
   to the C++ side (`Adapter -> UseCase, Ports`, never the reverse).
4. It remains a separate process/deployment from `seed_server` (C++) and
   the future `seed_auth` service, consistent with the Adjudicator's
   earlier instruction that these stay separate instances.
5. Session/token behavior (Bearer-only, no cookies, 1-hour TTL, 3-attempt
   lockout) is preserved as a behavioral contract, reimplemented in Kotlin
   rather than ported line-by-line from the C++ version.
6. Frontend: React + **Vite** (resolves LISS-0145/LISS-0149's open
   build-tooling question for both the admin SPA and the player
   registration/login SPA).
7. Deployment: **Docker Compose** brings up the Kotlin/Spring Boot backend
   and the Vite/React frontend, alongside the existing PostgreSQL service
   in `db/docker-compose.yml`.

## Resolved questions

- ~~Spring Data JDBC vs. JPA/Hibernate~~ — moot; MyBatis was chosen instead
  of either.
- ~~Build tool~~ — Gradle (Kotlin DSL).
- ~~Fate of existing C++ code~~ — kept until Kotlin replacement is verified,
  then discarded.

## Open question requiring explicit confirmation

The Adjudicator specified that **raw passwords/hashes travel only between
`seed_auth` and the game client**, with only one-time tokens crossing to
`seed_server` — this is clearly scoped to the *player* authentication flow
(ADR 0018 / LISS-0146-0147). It is not yet explicit whether the same
boundary statement also settles `seed_admin`'s own credential check.

Inferred default (not yet confirmed): since MyBatis is explicit-SQL (not an
ORM that hides queries), `seed_admin`'s Kotlin implementation calls
PostgreSQL's `pgcrypto` `crypt()` directly from a MyBatis mapper query,
exactly as the C++ version did — **no JVM password-hashing library is
introduced**, and the bootstrap admin account's existing `pgcrypto` hash in
`admin_users` remains valid without migration. This needs an explicit yes/no
before Phase 1 (Red) starts, since guessing wrong here would lock out the
bootstrap admin account.

Also still open: dependency-adoption checklist
(`docs/architecture/dependency-policy.md`) for Spring Boot, MyBatis,
Gradle, and their transitive dependencies has not been performed.

## Consequences (anticipated, pending approval)

Positive:

- Faster iteration on admin CRUD-style features going forward, per the
  Adjudicator's stated reasoning.
- Data/security policy already decided in ADR 0017 is reused unchanged,
  minimizing re-litigated ground.

Negative:

- The working, manually-verified C++ implementation (ADR 0017/LISS-0144) is
  kept dormant until the Kotlin replacement works, then discarded —
  duplicate maintenance burden during the transition.
- Introduces a second language runtime (JVM/Kotlin/Gradle) into a project
  that was previously C++-only, which is a meaningful new operational
  surface (build, deploy, monitor).
- `seed_auth` (LISS-0146, not yet built) will likely follow the same
  language choice for consistency, though that is not decided by this ADR.

## Related documents

- `docs/architecture/adr/0017-admin-authentication-and-http-surface.md`
  (superseded for implementation language only; data/security decisions
  remain in effect)
- `docs/issues/LISS-0144-admin-authentication-http-surface.md`
- `docs/issues/LISS-0145-admin-ui-react-spa.md`

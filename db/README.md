# Local PostgreSQL (LISS-0143/0144 / ADR 0016/0017)

Development-only PostgreSQL instance for `PostgresIdentityAliasStore` and
`AdminAuthStore`.

## Start

```sh
cd db
docker compose up -d
```

The `identity_aliases` table (`db/migrations/0001_identity_aliases.sql`), the
`admin_users` table (`db/migrations/0002_admin_users.sql`), and the registered-
player auth tables (`db/migrations/0003_player_challenges_sessions.sql`) are
applied automatically on first container start via `docker-entrypoint-initdb.d`. If
the named volume already existed from before a migration file was added,
apply it manually instead:

```sh
docker exec -i db-identity-postgres-1 psql -U seed_identity -d seed_identity \
  < migrations/0002_admin_users.sql
```

## Connection string

```sh
export SEED_IDENTITY_DB_URL="postgresql://seed_identity:seed_identity_dev_only@localhost:5432/seed_identity"
```

`PostgresIdentityAliasStore::fromEnvironment()` and
`AdminAuthStore::fromEnvironment()` both read this variable — `admin_users`
lives in the same `seed_identity` database (ADR 0017 decision 1). The
credentials in `docker-compose.yml` are for local development only and are
not used by any deployed environment.

## Bootstrapping the first admin account

There is no HTTP endpoint to create an admin account (ADR 0017 decision 6 —
an unauthenticated "create admin" endpoint would be a privilege-escalation
hole). Insert the first admin directly, using `pgcrypto`'s `crypt()` to
store a bcrypt hash, never a plaintext password:

```sh
docker exec db-identity-postgres-1 psql -U seed_identity -d seed_identity -c \
  "INSERT INTO admin_users (username, password_hash) VALUES ('<username>', crypt('<password>', gen_salt('bf')));"
```

## Building with the PostgreSQL adapter and admin HTTP surface

`libpqxx` (and its `libpq` dependency) are keg-only on Homebrew, so
`pkg-config` needs an explicit hint. `cpp-httplib` ships a CMake config
package instead, so it needs `CMAKE_PREFIX_PATH`:

```sh
export PKG_CONFIG_PATH="$(brew --prefix libpq)/lib/pkgconfig:$(brew --prefix libpqxx)/lib/pkgconfig:$PKG_CONFIG_PATH"
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix cpp-httplib)"
cmake --build build
```

If `libpqxx` is not found, CMake skips `seed_postgres`, `seed_postgres_tests`,
and `seed_admin`, and the rest of the build is unaffected. If `libpqxx` is
found but `cpp-httplib` is not, only `seed_admin` is skipped.

## Running the admin HTTP API

```sh
./build/seed_admin 40080
```

```sh
TOKEN=$(curl -s -X POST http://127.0.0.1:40080/login \
  -d "username=<username>&password=<password>" | sed -E 's/.*"token":"([^"]+)".*/\1/')
curl -s -H "Authorization: Bearer $TOKEN" http://127.0.0.1:40080/aliases/export
curl -s -H "Authorization: Bearer $TOKEN" http://127.0.0.1:40080/aliases/review-queue
curl -s -H "Authorization: Bearer $TOKEN" -X POST http://127.0.0.1:40080/aliases/review \
  -d "claimedId=<canonical-claimed-id>&status=confirmed&confidence=0.9"
curl -s -H "Authorization: Bearer $TOKEN" -X POST http://127.0.0.1:40080/logout
```

`seed_admin` binds to `127.0.0.1` only; there is no TLS termination and no
browser UI yet (see ADR 0017 follow-ups).

## Stop

```sh
cd db
docker compose down
```

Add `-v` to also delete the named volume (`identity-postgres-data`) and lose
local data.

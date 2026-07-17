# Local PostgreSQL (LISS-0143 / ADR 0016)

Development-only PostgreSQL instance for `PostgresIdentityAliasStore`.

## Start

```sh
cd db
docker compose up -d
```

The `identity_aliases` table (`db/migrations/0001_identity_aliases.sql`) is
applied automatically on first container start via
`docker-entrypoint-initdb.d`.

## Connection string

```sh
export SEED_IDENTITY_DB_URL="postgresql://seed_identity:seed_identity_dev_only@localhost:5432/seed_identity"
```

`PostgresIdentityAliasStore::fromEnvironment()` reads this variable. The
credentials in `docker-compose.yml` are for local development only and are
not used by any deployed environment.

## Building with the PostgreSQL adapter

`libpqxx` (and its `libpq` dependency) are keg-only on Homebrew, so
`pkg-config` needs an explicit hint:

```sh
export PKG_CONFIG_PATH="$(brew --prefix libpq)/lib/pkgconfig:$(brew --prefix libpqxx)/lib/pkgconfig:$PKG_CONFIG_PATH"
cmake -S . -B build
cmake --build build
```

If `libpqxx` is not found, CMake skips `seed_postgres` and
`seed_postgres_tests` and the rest of the build is unaffected.

## Stop

```sh
cd db
docker compose down
```

Add `-v` to also delete the named volume (`identity-postgres-data`) and lose
local data.

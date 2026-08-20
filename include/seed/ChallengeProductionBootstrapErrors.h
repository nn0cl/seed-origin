#ifndef SEED_CHALLENGE_PRODUCTION_BOOTSTRAP_ERRORS_H
#define SEED_CHALLENGE_PRODUCTION_BOOTSTRAP_ERRORS_H

namespace server {

constexpr const char* kMissingIdentityDbUrlError =
    "seed_server: SEED_CHALLENGE_AUTH requires SEED_IDENTITY_DB_URL";

constexpr const char* kPostgresBootstrapUnavailableError =
    "seed_server: SEED_CHALLENGE_AUTH is set but Postgres "
    "challenge adapters are not wired yet (LISS-0147)";

}

#endif

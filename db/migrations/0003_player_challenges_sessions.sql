-- LISS-0146/0147: registered-player challenge claim and session keys.
-- Canonical for seed_server Postgres adapters; seed_auth owns user creation.

CREATE EXTENSION IF NOT EXISTS pgcrypto;

CREATE TABLE IF NOT EXISTS users (
    id BIGSERIAL PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS player_challenges (
    challenge_key TEXT PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users (id),
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    expires_at TIMESTAMPTZ NOT NULL,
    claimed_at TIMESTAMPTZ
);

CREATE TABLE IF NOT EXISTS player_sessions (
    session_token TEXT PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users (id),
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    expires_at TIMESTAMPTZ NOT NULL,
    revoked_at TIMESTAMPTZ
);

CREATE INDEX IF NOT EXISTS player_challenges_unclaimed_expiry_idx
    ON player_challenges (expires_at)
    WHERE claimed_at IS NULL;

CREATE INDEX IF NOT EXISTS player_sessions_active_idx
    ON player_sessions (expires_at)
    WHERE revoked_at IS NULL;

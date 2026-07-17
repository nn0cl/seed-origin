CREATE TABLE IF NOT EXISTS identity_aliases (
    alias_id BIGINT PRIMARY KEY,
    canonical_claimed_id TEXT NOT NULL UNIQUE,
    created_tick BIGINT NOT NULL,
    last_used_tick BIGINT NOT NULL,
    confidence REAL NOT NULL CHECK (confidence >= 0.0 AND confidence <= 1.0),
    review_status SMALLINT NOT NULL DEFAULT 0
);

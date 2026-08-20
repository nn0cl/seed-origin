-- LISS-0148: classic MMORPG player progression + master data.
-- Event-driven player state; HP/MP/position remain runtime-only.

CREATE EXTENSION IF NOT EXISTS pgcrypto;

CREATE TABLE IF NOT EXISTS player_characters (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users (id),
    name TEXT NOT NULL,
    job_class_id INT NOT NULL,
    base_level INT NOT NULL DEFAULT 1,
    base_exp BIGINT NOT NULL DEFAULT 0,
    job_level INT NOT NULL DEFAULT 1,
    job_exp BIGINT NOT NULL DEFAULT 0,
    stamina INT NOT NULL DEFAULT 100
);

CREATE INDEX IF NOT EXISTS player_characters_user_id_idx
    ON player_characters (user_id);

CREATE TABLE IF NOT EXISTS master_base_exp_curve (
    level INT PRIMARY KEY,
    exp_to_next BIGINT NOT NULL
);

CREATE TABLE IF NOT EXISTS master_job_exp_curve (
    level INT PRIMARY KEY,
    exp_to_next BIGINT NOT NULL
);

CREATE TABLE IF NOT EXISTS master_item_templates (
    id BIGINT PRIMARY KEY,
    max_durability INT NOT NULL,
    socket_count INT NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS equipment_instances (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    character_id BIGINT NOT NULL REFERENCES player_characters (id),
    item_template_id BIGINT NOT NULL REFERENCES master_item_templates (id),
    durability INT NOT NULL,
    equipment_exp BIGINT NOT NULL DEFAULT 0
);

CREATE INDEX IF NOT EXISTS equipment_instances_character_id_idx
    ON equipment_instances (character_id);

CREATE TABLE IF NOT EXISTS equipment_sockets (
    equipment_instance_id UUID NOT NULL REFERENCES equipment_instances (id),
    socket_index INT NOT NULL,
    socketed_item_instance_id UUID REFERENCES equipment_instances (id),
    PRIMARY KEY (equipment_instance_id, socket_index)
);

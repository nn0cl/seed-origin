# LISS-0148: プレイヤー特性・アイテムのPostgres永続化スキーマ

- Status: in_progress
- Phase: phase-2-green
- Related branch: `feature/liss-0148-player-progression-persistence`
- Priority: medium
- Depends on: LISS-0146（done）
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`,
  `docs/architecture/adr/0023-player-auth-session-flow-details.md`
- Spec: `docs/specs/player-progression-v1.md`

## 目的

`user_id`に紐づくプレイヤーのレベル・経験値・アイテム・所持品等を
PostgreSQLへ永続化するスキーマと読み書きアダプタを追加する。

## 現状

`Player`/`Status`（HP・MP・位置・Buffのみ）はインメモリの`Field`
シングルトンにのみ存在し、レベル・アイテムの概念自体がまだ存在しない。

## ドメインモデルの方向性（2026-07-22確定、ADR 0023決定7）

レベル・経験値・アイテムのドメインモデルは、少し古いデザインのクラシックな
王道MMORPGのスタイルで定義する。ベースレベルと職業レベルは独立したEXP
テーブルを持ち、装備は固有UUIDを持つインスタンスとして管理する。装備
インスタンスには耐久値、装備EXP、装備スロット（アイテムをはめ込む構造）を
持たせ、キャラクターにはスタミナを持たせる。経験値テーブル等のバランス
調整値はハードコードせず、`seed_admin`から編集できるマスターデータとして
プレイヤー個別状態から分離する。

## Adjudicator decisions（2026-08-20）

一般的構成・推奨で確定:

1. **永続化頻度**: イベント駆動（経験値取得、アイテム消費・装備変更、
   スタミナ更新など）。定期フルスナップショットや logout のみではない。
2. **高頻度ランタイム状態**: HP / MP / 位置 / Buff は **メモリのみ**
   （毎 tick 永続化しない）。再接続は既存セッション/スナップショット経路。
3. **実装境界**: C++ `seed_server` の Port + `seed_postgres` Adapter。
4. **Phase 1 スライス**: 受入シナリオ + Postgres Adapter Red（migration /
   実装は Green）。

## 想定スキーマ（Green で migration 化）

プレイヤー状態:

- `player_characters` — `user_id` FK、名前、base/job level+exp、stamina、job_class_id
- `equipment_instances` — UUID、character_id、item_template_id、durability、equipment_exp
- `equipment_sockets` — equipment_instance_id、socket_index、socketed_item_instance_id

マスター:

- `master_base_exp_curve` / `master_job_exp_curve` — level → exp_to_next
- `master_item_templates` — id、max_durability、socket_count 等

## Phase 1 Red（2026-08-20）

- Spec: `docs/specs/player-progression-v1.md`
- Tests: `tests/PostgresPlayerProgressionStoreTest.cpp`（`seed_postgres_tests`）
- Covered: base/job EXP 独立更新、装備インスタンス、ソケット、スタミナ、
  マスター EXP 読み取り
- Expected Red: compile failure（`PostgresPlayerProgressionStore` 未実装）
- Out of Red: migration `0004_*`、Adapter 実装、Field/Player ドメイン拡張（後続）

## Phase 2 Green（2026-08-20）

- Migration: `db/migrations/0004_player_progression.sql`
- Adapter: `PostgresPlayerProgressionStore`（create/load character、base/job EXP、
  stamina、equipment instance/socket、master EXP/item template helpers）
- Verification: progression adapter 6 scenarios PASS with `SEED_IDENTITY_DB_URL`
  （full `seed_postgres_tests` は既存 bootstrap ケースが環境依存で assert しうる）

## English

Persist classic-MMORPG player progression in PostgreSQL keyed by user/character,
with event-driven writes and master-data separation (ADR 0023 decision 7).

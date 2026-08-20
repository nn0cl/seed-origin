# Player Progression Persistence v1

## Purpose

LISS-0148 / ADR 0023 decision 7 のクラシック MMORPG 型プレイヤー進行を、
PostgreSQL に永続化するための受入シナリオ。HP/MP/位置の毎 tick 永続化は
対象外（ランタイムメモリ）。進行イベント発生時に永続化する。

## Actors

| Actor | Role |
| --- | --- |
| `seed_server` | 進行イベント適用・Postgres Adapter 経由の永続化 |
| PostgreSQL | プレイヤー状態 + マスターデータ |
| `seed_admin` | マスターデータ編集（本スライスでは読み取り契約のみ） |

## Persistence policy（Adjudicator 2026-08-20）

- **イベント駆動**: 経験値取得、アイテム消費・装備変更、スタミナ更新など
  意味のある進行イベントで書き込む。
- **非永続（メモリ）**: HP、MP、ワールド位置、Buff 寿命（毎 tick は書かない）。
- **実装境界**: C++ `seed_server` ポート + `seed_postgres` Adapter。
- **マスター分離**: EXP 曲線・アイテムテンプレートはプレイヤー状態と別テーブル。

## Gherkin

### Character + independent EXP

```gherkin
Feature: Player character progression persistence

  Scenario: Granting base experience updates only base progression
    Given a registered user with a persisted character
    When the server grants base experience on a kill/reward event
    Then the character's base_exp (and base_level when threshold crossed) is updated
    And job_exp and job_level are unchanged

  Scenario: Granting job experience updates only job progression
    Given a registered user with a persisted character
    When the server grants job experience on a job-relevant event
    Then the character's job_exp (and job_level when threshold crossed) is updated
    And base_exp and base_level are unchanged
```

### Equipment instances

```gherkin
  Scenario: Creating an equipment instance persists UUID durability and equipment EXP
    Given a persisted character and an item template in master data
    When the server creates an equipment instance for that character
    Then a row exists with a unique instance id, durability, and equipment_exp

  Scenario: Socketing an item into equipment persists the socket link
    Given an equipment instance with available sockets
    When the server sockets an item instance into a socket index
    Then the socket relationship is persisted for that equipment instance
```

### Stamina

```gherkin
  Scenario: Stamina changes are persisted on stamina events
    Given a persisted character
    When the server applies a stamina change event
    Then the character's stamina value is updated in Postgres
```

### Master data separation

```gherkin
  Scenario: EXP thresholds come from master data not hard-coded adapter logic
    Given base EXP curve rows in master data
    When the adapter loads the threshold for a base level
    Then the value is read from the master table
```

## Related

- `docs/issues/LISS-0148-player-progression-persistence-schema.md`
- `docs/architecture/adr/0023-player-auth-session-flow-details.md` (decision 7)

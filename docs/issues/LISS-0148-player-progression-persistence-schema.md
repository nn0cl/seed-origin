# LISS-0148: プレイヤー特性・アイテムのPostgres永続化スキーマ

- Status: proposed
- Phase: phase-1-red
- Priority: medium
- Depends on: LISS-0146
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

`user_id`に紐づくプレイヤーのレベル・経験値・アイテム・所持品等を
PostgreSQLへ永続化するスキーマと読み書きアダプタを設計する。

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

## Phase 1 設計着手

ベース／職業EXPの独立更新、装備インスタンスのUUID・耐久値・装備EXP・ソケット、
プレイヤーのスタミナ、管理可能なマスターデータを対象にRedテストと受入シナリオ
を設計する。

## 設計課題（Must not guess）

- レベル・経験値・アイテムの詳細な項目定義。
- 高頻度で変わるHP/MP/位置と、低頻度で変わるレベル/アイテムを分離して
  永続化するか（毎tick全部書き込むのは非現実的）。
- `seed_admin`から編集可能なチューニング設定テーブルと、プレイヤー個別の
  進行状態テーブルを分離するスキーマ設計。
- ログアウト時のみ保存するのか、定期的にスナップショット保存するのか。

## Remaining decisions

- ドメインモデルの大方針と必須永続化要件は確定済み。Phase 1 Redのテスト
  レビュー後にPhase 2 Greenへ進む。

## English

Design intake for persisting player progression (level, items) in
PostgreSQL keyed by user_id. The domain model itself does not exist yet;
this Issue starts from design, not implementation.

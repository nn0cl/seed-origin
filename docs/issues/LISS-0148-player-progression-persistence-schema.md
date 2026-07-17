# LISS-0148: プレイヤー特性・アイテムのPostgres永続化スキーマ

- Status: proposed
- Phase: phase-0-design-intake
- Priority: medium
- Depends on: LISS-0146
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

`user_id`に紐づくプレイヤーのレベル・経験値・アイテム・所持品等を
PostgreSQLへ永続化するスキーマと読み書きアダプタを設計する。

## 現状

`Player`/`Status`（HP・MP・位置・Buffのみ）はインメモリの`Field`
シングルトンにのみ存在し、レベル・アイテムの概念自体がまだ存在しない。

## 設計課題（Must not guess）

- レベル・経験値・アイテムのドメインモデル自体が未設計（本Issueで
  ゼロから設計するか、別途ゲームデザイン仕様が必要か）。
- 高頻度で変わるHP/MP/位置と、低頻度で変わるレベル/アイテムを分離して
  永続化するか（毎tick全部書き込むのは非現実的）。
- ログアウト時のみ保存するのか、定期的にスナップショット保存するのか。

## Remaining decisions

- 本Issueは設計着手のみで、ドメインモデル自体が未定義のため、
  Adjudicatorとのゲームデザイン協議が前提となる。

## English

Design intake for persisting player progression (level, items) in
PostgreSQL keyed by user_id. The domain model itself does not exist yet;
this Issue starts from design, not implementation.

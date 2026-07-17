# LISS-0130: ID永続化と名寄せ

- Status: proposed
- Priority: high
- Depends on: LISS-0123, LISS-0053

## 目的

匿名セッションの過去ID名寄せを安全に永続化する。これは認証の代替ではなく、利用者の申告情報を扱うデータ機能として設計する。

## 受入条件

- 内部ID、申告ID、作成・最終利用時刻を分離する。
- 名寄せの確信度、衝突時の人間判断、解除手段を記録する。
- 平文ログ・公開APIから申告IDを漏洩させない。
- 削除、エクスポート、保持期間、バックアップを定義する。
- 認証導入時に移行できる。

## English

Persist anonymous identity aliases safely without treating them as authentication. Separate internal IDs and claims, define confidence, human adjudication, privacy, deletion, retention, backup, and future authentication migration.

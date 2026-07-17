# LISS-0130: ID永続化と名寄せ

- Status: in_progress
- Phase: phase-2-green
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

## Implementation slice

- `IdentityAliasStore`ポートを追加し、内部ID・申告ID名寄せを外部Storeから分離した。
- 現在の既定実装は`InMemoryIdentityAliasStore`であり、DB・ファイル・ネットワークを
  選択せず、既存のインメモリBaselineを維持する。
- 名寄せレコードはcanonical申告ID、alias ID、created/last-used WorldTick、確信度、
  人間レビュー状態を持つ。申告IDは認証情報として扱わない。
- `SessionRegistry`はStore注入、エクスポート、明示的な申告ID削除を提供する。
  削除しても既存のactive internal sessionは変更しない。
- 注入Store内の既存alias IDを調査して次の採番値を決め、再起動・移行時のID衝突を避ける。

## Remaining decisions

- 実永続化エンジン、保存形式、暗号化、バックアップ、保持期間、エクスポート認可、
  人間による衝突解決UIは未決定であり、別ADR/Issueで決定する。
- 平文ログ・公開Snapshot・Command応答へcanonical申告IDを出力しない契約は維持する。

## Verification

- Store注入、大小文字をまたぐ名寄せ、metadata更新、明示削除、active session非変更の
  テスト資料を追加した。
- `git diff --check`と警告有効C++20ビルドのみ実行。テスト、成果物、サーバーは未実行。

## English

Persist anonymous identity aliases safely without treating them as authentication. Separate internal IDs and claims, define confidence, human adjudication, privacy, deletion, retention, backup, and future authentication migration.

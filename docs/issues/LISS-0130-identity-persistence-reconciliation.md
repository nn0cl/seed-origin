# LISS-0130: ID永続化と名寄せ

- Status: superseded
- Superseded by: ADR 0018 / LISS-0150（2026-08-20）
- Phase: superseded（no further Phase 1/2/3 work）
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
- `IdentityAliasStore::reviewAlias`と`SessionRegistry::recordAliasReview`を追加し、
  衝突時の人間判断（レビュー状態の確定・却下）と確信度の更新手段を提供した。
  対象レコードが存在しない、または確信度が[0,1]範囲外の場合は状態を変更せず失敗を返す。
  レビュー結果の記録もactive internal sessionに影響しない。

## Remaining decisions

- 実永続化エンジン、保存期間、エクスポート方式、衝突解決UI、暗号化方針は
  `docs/architecture/adr/0016-identity-alias-persistence-and-review.md`で決定した:
  PostgreSQL、保存期間は設定可能（既定2年、`lastUsedTick`起点）、
  管理者ログイン必須のエクスポート専用ページ、キュー型衝突解決UI、
  申告IDはニックネームのみで連絡先情報を含めず、暗号化はPostgreSQL側の
  at-rest暗号化とアクセス制御・監査ログを主軸とする。
- ADR 0016が持ち越したambiguity: (1) `lastUsedTick`（WorldTick）から
  実時間の保存期間を算出する対応関係が未設計、(2) エクスポート/衝突解決
  キューを守る管理者認証機構、(3) PostgreSQLドライバ選定と
  依存adoption checklistの脆弱性確認等が未実施。(2)は
  `docs/architecture/adr/0017-admin-authentication-and-http-surface.md`
  （LISS-0144）で決定・実装した。(1)(3)は引き続き未解決。
- `CLAUDE.md`の「Current Non-Decisions」の「Identity persistence
  format/backend」表記は、エンジン決定（PostgreSQL）を反映するよう更新が
  必要だが、これはagent operating contractの変更であり
  `docs/collaboration/prompt-instruction-change-control.md`の手続き
  （traceファイル・Adjudicator明示レビュー）を経て別途行う。
- 平文ログ・公開Snapshot・Command応答へcanonical申告IDを出力しない契約は維持する。

## Verification

- Store注入、大小文字をまたぐ名寄せ、metadata更新、明示削除、レビュー状態の確定・却下、
  存在しないaliasや不正な確信度の拒否、active session非変更のテスト資料を追加した。
- 警告有効C++20ビルドを実行し成功を確認した。Adjudicator方針によりテスト実行
  （ctest）は保留し、成果物・サーバーは未実行。

## Supersession（LISS-0150、2026-08-20）

- プレイヤー認証は登録制（ADR 0018）へ移行済み。本Issueの続き（purge、追加
  retention 実装等）には着手しない。
- 本Issueは削除せず `superseded` として残す。
- `IdentityAliasStore` / `PostgresIdentityAliasStore` コードは当面残置
  （Admin alias review API 等が参照）。削除は ADR 0023 決定5どおり
  `seed_auth` と `seed_admin` Kotlin 移行完了後。

## English

Persist anonymous identity aliases safely without treating them as authentication. Separate internal IDs and claims, define confidence, human adjudication, privacy, deletion, retention, backup, and future authentication migration. Formally superseded by ADR 0018 / LISS-0150.

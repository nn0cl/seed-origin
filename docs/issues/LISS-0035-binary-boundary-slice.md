# LISS-0035: SeedBinary/FileManager 境界安全化スライス

## Metadata

- Local issue ID: LISS-0035
- Parent issue: LISS-0025
- Status: review
- Phase: phase-1-red
- Type: safety + bugfix
- Priority: critical
- Related branch: `main`

## Scope

`SeedBinary` の入力境界と初期化、`FileManager` の読み書き失敗の伝播を一つのスライスとして修正する。

## Acceptance criteria

- 1024バイトを超える入力、null入力、負のブロック番号を拒否する。
- ブロックの未使用領域を初期化し、未初期化メモリを読み出さない。
- ファイルサイズ取得、ブロック登録、ブロック取得、write、flush、close の失敗を呼び出し元へ返す。
- 欠損または無効なブロックを黙って出力せず、保存を失敗させる。
- `rw` の未使用引数をAPIから除去し、読み込み契約を明確にする。
- 境界値・欠損ブロック・入出力失敗のテストコードを追加する。

## Verification note

テストコードはTDD資料として追加した。ユーザー承認により実装は進めたが、移行安全性ゲートの運用規約により、このスライスではコンパイル・テスト実行を行わない。

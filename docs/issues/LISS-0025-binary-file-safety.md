# LISS-0025: SeedBinary・FileManagerの境界安全性

## Metadata

- Local issue ID: LISS-0025
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: safety + bugfix
- Priority: critical
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

`SeedBinary` と `FileManager` のバッファ境界、インデックス、欠損ブロック、I/Oエラー処理を安全化する。

## Dependencies

- Parent: LISS-0017
- Depends on: LISS-0019
- Blocks: binary data features

## Findings

- `setBinary` が1024バイト超の入力を拒否しない。
- 負のインデックスを拒否しない。
- ブロックの未使用領域が初期化されない。
- `FileManager::save` が `getBinary` の失敗を確認せず、未初期化の `size` / `isValid` を使い得る。
- ファイルサイズ取得失敗時の扱いが未定義。
- `rw` 引数が使用されず、API契約が不明確。
- 書き込み失敗やclose失敗を返さない。

## Acceptance Notes

- 入力サイズ、インデックス、欠損ブロックを明示的に拒否または処理する。
- 読み書きの成功・失敗を呼び出し元が判定できる。
- バイナリ変換の同一性をテスト仕様化する。
- 不正入力がメモリ破壊を起こさない。
- Sanitizer実行は安全性ゲート後に行う。

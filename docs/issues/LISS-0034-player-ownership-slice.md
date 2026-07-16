# LISS-0034: Player所有権・コピー代入安全性スライス

## Metadata

- Local issue ID: LISS-0034
- GitHub issue: 未作成
- Status: review
- Phase: phase-3-refactor
- Type: safety + bugfix
- Priority: critical
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

LISS-0019の最初の実装スライスとして、`Player` が所有する状態のコピー・代入・破棄を安全化する。特に `Field::putPlayer` の同一ID更新で暗黙コピー代入が生ポインタを浅くコピーする問題を解消する。

## Dependencies

- Parent: LISS-0019
- Depends on: 承認済みのLISS-0019安全性ゲート
- Blocks: LISS-0026, LISS-0027, LISS-0028, world実装

## Acceptance Notes

- `Player a; Player b; b = a;` が所有状態を共有しない。
- `Field::putPlayer` の新規登録・同一ID更新・コンテナ破棄で二重解放が発生しない。
- コピー後のPlayer状態を独立して変更できる。
- null入力を暗黙のデフォルトPlayerへ置換しない。
- 所有ポインタを外部からdeleteできないAPI境界を定義する。
- 失敗する回帰テストを先に追加し、実装後に安全性検証を行う。

## Design decision

現行公開APIとの互換性を確認しながら、第一候補は `Status` と `Position` を値メンバー化する方式とする。互換性上必要な場合のみ、`std::unique_ptr` とRule of Fiveを採用する。どちらを採用するかは実装前に差分と影響範囲を確認する。

## Verification

- Phase 1: コピー代入、同一ID更新、独立変更、破棄順の回帰テストを追加する。
- Phase 2: AddressSanitizer/UndefinedBehaviorSanitizerを有効にした検証を行う。
- 今回のスライスはmainへコミット・プッシュするが、CodeQLはLISS-0031の依存関係により未設定である。
- Sanitizerとテストは今回実行していないため、Issueはレビュー状態で保持する。
- CodeQLはLISS-0031の依存関係により、既知不具合修正完了後まで未設定である。

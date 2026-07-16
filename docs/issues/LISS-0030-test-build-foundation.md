# LISS-0030: TDDテスト・ビルドターゲット基盤

## Metadata

- Local issue ID: LISS-0030
- GitHub issue: 未作成
- Status: review
- Phase: phase-1-red
- Type: test + build
- Priority: high
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

空のテスト、Xcodeの単一実行ターゲット、将来のDomain/Application/Adapterテスト配置を整理する。今回の計画段階ではテストを実行しない。

## Dependencies

- Parent: LISS-0017
- Depends on: LISS-0019
- Related: LISS-0020〜LISS-0029
- Active slice: LISS-0040

## Findings

- 既存テストは実質的なassertionがなく、戻り値0だけのものがある。
- `tests/legacy` に旧API互換の空スタブが残っている。
- Xcodeに独立したテストターゲットがない。
- 本体ターゲットとテストの責務が明確でない。
- Sanitizer用のテスト実行構成がない。

## Acceptance Notes

- Phase 1のRedテスト配置と命名規約を定義する。
- domainテストが外部I/Oや通信に依存しない。
- Xcodeと将来のCLI/CIで同じテスト対象を扱える設計にする。
- テスト実行は安全性ゲートとAdjudicator承認後に行う。

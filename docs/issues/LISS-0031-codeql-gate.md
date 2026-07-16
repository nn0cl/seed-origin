# LISS-0031: GitHub CodeQLレビューゲート設定

## Metadata

- Local issue ID: LISS-0031
- GitHub issue: 未作成
- Status: review
- Phase: phase-2-config
- Type: security + process
- Priority: critical
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

GitHub CodeQLを有効化し、mainへのpush後にレビュー結果を確認してから次Issueへ進む運用ゲートを設定する。

## Dependencies

- Parent: なし
- Depends on: LISS-0019, LISS-0025, LISS-0026, LISS-0027, LISS-0028, LISS-0030, LISS-0032, LISS-0033, mainブランチのGitHub側設定
- Blocks: Issue completion flow

## Acceptance Notes

- CodeQLのDefaultまたはAdvanced setupが有効になる。
- C/C++が分析対象になる。
- mainへのpush、main向けPR、週次スキャンの実行条件が確認できる。
- 指摘の有無をIssue完了報告へ記録できる。
- 指摘ありの場合に自動で次Issueへ進まない運用を定義する。
- CodeQLはメモリ安全性の唯一の証明ではなく、Sanitizer・レビューと併用する。

## Configuration

- Workflow: `.github/workflows/codeql.yml`
- Language: C/C++ (`c-cpp`)
- Build mode: `none`（現状はXcodeプロジェクトのみで、CI上の再現可能なビルド手順が未整備のため）
- Triggers: `main` push、main向けPull Request、毎週月曜日UTC 02:30
- Query suite: `security-extended`
- Results: GitHub Code Scanningへアップロード
- Active finding slice: LISS-0039
- Initial scan: completed; no open CodeQL findings remain (two fixed, one reviewed and dismissed as false positive).

## Execution Gate

ユーザー承認により、既知不都合の修正途中でもCodeQL設定を先行して有効化する。初回スキャン結果を確認し、指摘があれば新規Issue化して最優先で扱う。ビルドモードの改善はLISS-0030で別途行う。

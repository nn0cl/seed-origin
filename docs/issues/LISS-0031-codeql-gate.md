# LISS-0031: GitHub CodeQLレビューゲート設定

## Metadata

- Local issue ID: LISS-0031
- GitHub issue: 未作成
- Status: blocked
- Phase: phase-0-design
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

## Execution Gate

現在把握している安全性・正確性・未実装動作の修正が一通り完了し、関連IssueがCodeQL設定を開始できる状態になるまで、このIssueは実施しない。CodeQL設定を先行させない。

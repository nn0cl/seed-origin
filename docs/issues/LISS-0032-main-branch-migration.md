# LISS-0032: GitHub mainブランチ運用への移行

## Metadata

- Local issue ID: LISS-0032
- GitHub issue: 未作成
- Status: review
- Phase: phase-0-design
- Type: process + repository
- Priority: high
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

現在のGitHubリポジトリはmasterをデフォルトとしているため、mainを正式な作業・push先にするためのリモート設定を整える。

## Acceptance Notes

- `main` がリモートに存在する。
- GitHubのデフォルトブランチが `main` になる。
- CodeQLの対象ブランチと運用規約が一致する。
- mainへのpush権限、保護、Actions実行条件を確認する。
- 既存履歴とIssue参照が失われない。

## Resolution

`main`はリモートに存在し、GitHubのdefault branchも`main`であることを確認済み。CodeQLと作業規約の対象もmainに統一されている。

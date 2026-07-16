# LISS-0033: 旧API・空実装・レガシーソースの整理

## Metadata

- Local issue ID: LISS-0033
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: cleanup
- Priority: low
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

未使用の `ActionQueue`、空の `Manager`、`DiskManager`、`TestRoot`、旧テストスタブ、未使用APIを、削除・保留・実装のいずれかに分類する。

## Acceptance Notes

- 各空実装の利用箇所と将来仕様を確認する。
- 移行対象でない旧APIを無断で削除しない。
- 削除する場合は履歴と代替先を記録する。
- 残す場合は未実装契約を明示する。

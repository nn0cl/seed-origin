# LISS-0088: WorldUpdate sequence管理

- Status: review
- Phase: phase-1-red
- Type: reliability + protocol
- Priority: critical
- Parent: LISS-0080
- Related: LISS-0063, LISS-0084, LISS-0087

## Scope

クライアントまたはtransport adapterがWorldUpdateの順序を検証するため、初回Snapshot、連番、重複、欠落を状態機械として扱う。

## Acceptance criteria

- 初回Updateを現在のsequenceとして受理できる。
- 期待sequenceを受理すると次のsequenceへ進む。
- 既に処理したsequenceはDuplicateとして破棄できる。
- 未来sequenceはGapとして検出し、期待値を進めない。
- 不正EnvelopeはInvalidとして扱う。
- Snapshot再同期後に`resetAfterSnapshot`で追跡を再開できる。
- Gap時に推測でWorld状態を更新しない。

sequence trackerを実装した。再同期要求のtransport処理とSnapshot適用は後続Issueで扱う。テスト・ビルドは運用規約により実行していない。

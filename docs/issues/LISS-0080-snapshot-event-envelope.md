# LISS-0080: Snapshot/Event envelope契約

- Status: in_progress
- Phase: phase-0-design
- Type: architecture + protocol
- Priority: critical
- Parent: LISS-0049
- Related: LISS-0055, LISS-0063, LISS-0079

## Scope

20Hzのサーバーフレーム結果を、権威状態であるSnapshotと、発生事象であるEventに分離してクライアントへ伝える契約を定義する。

## Acceptance criteria

- `worldTick`、単調なsequence、event ID、server timeの意味を定義する。
- SnapshotにPlayer/NPC/対象物/位置/HP/エーテルなどの公開状態を定義する。
- Eventに発生元・対象・種別・演出用パラメータを定義する。
- Eventだけから権威状態を復元しない。
- 重複排除、欠落検出、Snapshot再同期要求を定義する。
- v1 wire formatへ追加するフィールドと互換性方針を記録する。

Active slice: LISS-0087（WorldUpdate Envelopeの型と検証）, LISS-0088（sequence管理）。

# LISS-0082: クライアントSnapshot反映と補間

- Status: proposed
- Phase: phase-2-integration
- Type: feature
- Priority: high
- Parent: LISS-0063
- Related: LISS-0065, LISS-0080

## Scope

サーバー20HzのSnapshotを、クライアントの描画FPSとは独立して状態へ適用し、必要な位置・表示状態を補間する。

## Acceptance criteria

- Snapshotを`worldTick`/sequence順に検証する。
- 重複Snapshotを安全に破棄する。
- 欠落を検出して再同期を要求する。
- 表示補間は権威状態を変更しない。
- 遅延・再接続後に最新Snapshotへ収束する。

# LISS-0023: World runtimeと時間進行

## Metadata

- Local issue ID: LISS-0023
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: feature
- Priority: high
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

worldシミュレーションの開始、停止、固定時間進行、プレイヤー状態集計を定義する。スレッドの多重起動、停止不能、処理遅延による非決定性を防ぐ。

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0022
- Blocks: LISS-0024

## Acceptance Notes

- 開始・停止を繰り返してもワーカーが増殖しない。
- 固定時間ステップと実時間の遅延ポリシーが定義されている。
- 処理時間がステップを超えた場合の扱いが決まっている。
- 状態集計がシミュレーション状態と一貫する。
- 20 FPS（50ms固定ステップ）でフレーム境界を管理する。
- フレーム開始時にActionQueueをスワップし、処理中の新規Actionを次フレームへ分離する。
- ランタイムテストはPhase 1で作成するが、今回は実行しない。

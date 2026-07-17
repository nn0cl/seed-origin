# LISS-0086: 論理Server tickとActionQueue接続

- Status: review
- Phase: phase-1-red
- Type: architecture + determinism
- Priority: critical
- Parent: LISS-0081
- Related: LISS-0079, LISS-0080, LISS-0058

## Scope

ActionQueueを20Hzの論理フレームへ接続し、各フレームに`worldTick`と確定済みAction列を渡す。実時間clock、sleep、World副作用は後続のRuntime/Worldスライスで扱う。

## Acceptance criteria

- 1論理フレームを50msとして定義する。
- `worldTick`が単調に増加する。
- フレーム進行時にActionQueueを切り替え、確定Actionを一度だけ返す。
- 空フレームを安全に処理する。
- 処理中到着分を次の`advanceFrame`へ残す。
- 固定clock実装とWorld適用を分離する。

論理tickとActionQueueの接続を実装した。テスト・ビルドは運用規約により実行していない。

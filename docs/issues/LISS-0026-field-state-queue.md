# LISS-0026: Fieldの状態・キュー契約と未実装処理

## Metadata

- Local issue ID: LISS-0026
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: bugfix + refactor
- Priority: high
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: `main`

## Summary

`Field` のプレイヤー更新、削除、アクション処理、キュー排他、シングルトン状態を仕様化する。

## Dependencies

- Parent: LISS-0017
- Depends on: LISS-0019
- Related: LISS-0022, LISS-0023

## Findings

- `unsetPlayer` が常に成功を返すだけで削除しない。
- `gainStatus` が何もせず成功を返す。
- アクション種別の大半が未処理。
- `processFrame` のプレイヤー走査が空処理。
- `std::map<int, Player>` が `int64_t` のプレイヤーIDを切り捨てる。
- キュー操作に排他契約がなく、同時操作の安全性がない。
- 未登録プレイヤーの位置更新を黙って破棄する。
- `fieldInstance` の宣言とローカルstatic実装が二重で、設計が不明確。

## Acceptance Notes

- 追加・更新・削除の戻り値と冪等性を定義する。
- キューの所有権、処理順、排他、満杯時の扱いを定義する。
- 全アクション種別の副作用を仕様化する。
- ID型を統一する。
- `Field` をworldシミュレーションと混同しない責務境界を維持する。

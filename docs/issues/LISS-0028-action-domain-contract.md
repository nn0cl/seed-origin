# LISS-0028: Action APIとドメイン結果契約

## Metadata

- Local issue ID: LISS-0028
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: refactor + feature
- Priority: high
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

`Action` の種別、実行者・対象者、Status参照、失敗理由、イベント結果を明確なドメイン契約へ整理する。

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0019, LISS-0020
- Blocks: LISS-0022

## Findings

- `actionType` が整数のマジックナンバー。
- nullの実行者・対象者をデフォルトPlayerへ置換し、入力ミスを隠す。
- getterが内部Player/Statusへの変更可能ポインタを返す。
- 成功・拒否・対象欠損・射程外の結果型がない。
- 直接効果と環境エーテル作用を表現できない。

## Acceptance Notes

- 種別と結果理由が型または明示的な値で表現される。
- 不正な実行者・対象者を暗黙のデフォルトへ置換しない。
- 行動結果、対象効果、環境作用、二次イベントを追跡できる。
- イベント連番と実行時ワールド状態が記録される。

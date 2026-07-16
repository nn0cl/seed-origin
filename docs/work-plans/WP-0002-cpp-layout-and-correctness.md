# WP-0002: C++構成と既知不具合の修正

## Scope

LISS-0017 に対応する単一イシュー計画。現在の直下ソースを一般的なC++構成へ移し、Xcode参照と既知の correctness / ownership 問題を修正する。

## Plan

| Order | Work item | Issue | Status | Evidence |
| --- | --- | --- | --- | --- |
| 1 | LISS-0017の設計・受入条件を確定 | LISS-0017 | in_progress | issue file |
| 2 | `src/`・`include/`・`tests/` へ配置しXcode参照を更新 | LISS-0017 | pending | `git diff`, project reference check |
| 3 | Data / Status / Position / Field の明らかな不具合を修正 | LISS-0017 | pending | compile or focused checks |
| 4 | 所有権・mainの到達不能処理・固定パスを整理 | LISS-0017 | pending | compile or focused checks |
| 5 | 検証しIssue/Planを同期 | LISS-0017 | pending | final status and verification |

## Current Next Issue

- LISS-0017: 完了条件を確認後、Phase 2相当の実装へ進む。

## Decisions and Ambiguities

- 既存の古いXcode形式は可能な限り壊さず、参照パスだけ更新する。
- `seeds` 系の `Action` / `Player` / `Field` は現行ソースとして残すが、未使用APIの設計変更は行わない。
- 外部依存やネットワーク実装の追加は行わない。

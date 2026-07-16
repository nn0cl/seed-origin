# LISS-0022: 決定論的イベント・効果解決

## Metadata

- Local issue ID: LISS-0022
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: feature
- Priority: high
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: `main`

## Summary

ログイン、移動、魔法、攻撃、命中、対象効果、環境エーテル注入、属性相互作用による二次イベントを、時刻と連番で決定論的に解決する。

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0020, LISS-0021
- Blocks: LISS-0023

## Acceptance Notes

- イベントに発生時刻、連番、発生元、対象、属性、効果量がある。
- 同時刻イベントの順序が固定される。
- 実行時点の環境エーテルを参照して魔法威力を決める。
- 直接効果と環境作用が別イベントとして追跡できる。
- 空キュー、未知コマンド、欠損対象、範囲外座標を安全に扱う。
- 状態異常・ダメージ・エーテル注入・二次イベントのテストをPhase 1成果物にする。
- テストは今回実行しない。

## Adjudicator Decision Points

- イベントキューを単一キューにするか、入力・解決・副作用で分けるか。
- 同一ティック内の環境更新と行動解決の順序。
- イベント再生とログ保存を初期スコープに含めるか。

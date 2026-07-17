# LISS-0114: 環境エーテル・伝導率・減衰モデル

- Status: review
- Phase: phase-1-red
- Type: feature + deterministic simulation
- Priority: high
- Parent: LISS-0022
- Depends on: LISS-0113, LISS-0104

## 目的

属性ごとの環境エーテル値をWorldへ保持し、エーテルの絶対値に応じて伝導率を上げる。
Spellは現在値の伝導率で威力補正され、使用属性の環境エーテルを増加させる。各20FPS
World frameの処理後に全属性を減衰させる。

## 現行仕様

- 属性: fire / water / earth / air
- 値の範囲: -100000〜100000
- 伝導率: `1.0 + min(abs(value) / 100.0, 1.0)`
- 減衰: frameごとに`value *= 0.98`
- Spell影響: 使用前の伝導率で威力補正し、`basePower * 0.1`を環境へ加算

## 受入条件

- 未知属性を拒否する。
- 正負どちらのエーテル偏りでも伝導率が上がる。
- Spell適用時に補正威力と環境エーテル変化が決定論的である。
- frame処理後にエーテルが減衰する。
- Combatは環境エーテルを変更しない。
- 属性相互作用、閾値による悪影響、MP消費、耐性、永続化は後続Issueで扱う。

## 実装資料

`EnvironmentEther`を追加し、FieldのSpell適用へ伝導率補正・環境加算・frame減衰を接続
した。テスト・ビルドは実行していない。

## 次のIssue

属性間相互作用とエーテル偏りによる副作用を仕様化し、Spell結果とWorldUpdateへ反映する。

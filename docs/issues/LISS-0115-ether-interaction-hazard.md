# LISS-0115: 属性間エーテル相互作用と副作用強度

- Status: review
- Phase: phase-1-red
- Type: feature + deterministic simulation
- Priority: high
- Parent: LISS-0022
- Depends on: LISS-0114, LISS-0113

## 目的

Spellで増加する属性エーテルが対属性へ反作用し、環境全体の偏りから不安定度と副作用
強度を算出できるようにする。副作用の具体的な対象変更は別のWorld仕様で決める。

属性の意味と未採用の循環伝播を含む全体ルールは
[`docs/architecture/ether-rules.md`](../architecture/ether-rules.md)に集約する。

## 現行仕様

- fire↔water、earth↔airを対属性とする。
- Spellの環境影響`basePower * 0.1`を使用属性へ加算する。
- 同じ影響の25%を対属性へ減算する。
- 不安定度は全属性の絶対値合計を200で割り、最大10に制限する。
- 副作用強度は`max(0, instability - 1)`とする。
- Fieldはframe後の副作用強度をhazard値として観測可能にする。

## 受入条件

- 対属性反作用が決定論的である。
- 正負どちらの偏りも不安定度へ反映される。
- 閾値を超えたときhazardが正になる。
- Combatはエーテル相互作用を発生させない。
- hazardをPlayer／NPC／対象物へ適用する処理はこのIssueへ混入させない。

## 実装資料

対属性反作用、不安定度、副作用強度、Fieldのframe hazard観測値を追加した。
テスト・ビルドは実行していない。

## 次のIssue

LISS-0116でhazardをWorldUpdate Eventへ表現した。対象物への具体的な影響とSnapshot集約は
後続Issueで扱う。

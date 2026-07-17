# LISS-0112: 攻撃・魔法Intentの入力境界

- Status: review
- Phase: phase-1-red
- Type: feature + security
- Priority: high
- Parent: LISS-0046
- Depends on: LISS-0109, LISS-0110, LISS-0111

## 目的

Attack／CastSpell Commandを、対象ID・威力・属性を検証済みのCombatIntent／SpellIntentへ
変換し、WorldInputQueueへ共通sequenceで蓄積する。受信時にFieldやCombat結果を変更しない。

## 受入条件

- Attack payload `targetId,power`を検証できる。
- Spell payload `targetId,element,power`を検証できる。
- actor／target IDが正、威力が有限かつ正で上限内である。
- Spell属性が許可された文字集合・長さである。
- AttackとSpellが共通WorldInputQueueで到着順を保持する。
- 受信時に命中、距離、Player HP、環境エーテルを変更しない。

## 実装資料

`CombatCommandHandler`、Combat／Spell Intent型、WorldInputQueue投入、WorldUpdate Eventの
最小payloadを追加した。WorldFrameApplierは入力検証とEvent化まで行い、ゲーム意味論は
未実装である。テスト・ビルドは実行していない。

## 次のIssue

LISS-0113でCombat／Spell Intentの対象存在・距離・威力検証と決定論的ダメージ適用を
World frameへ追加した。環境エーテル効果は後続Issueで扱う。

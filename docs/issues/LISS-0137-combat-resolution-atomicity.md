# LISS-0137: 攻撃・魔法の確定結果と原子性

- Status: proposed
- Phase: phase-1-red
- Type: feature + security + determinism
- Priority: critical
- Depends on: LISS-0126, LISS-0115, LISS-0120

## 目的

攻撃・魔法の入力受付と確定結果を分離し、HP、MP、エーテル、クールダウンを部分適用なく更新する。クライアントへは入力の反映ではなく、サーバーが確定した結果をWorldUpdate Eventとして配信する。

## 受入条件

- 攻撃者・対象・射程・威力・属性・リソース・クールダウンを全件検証してから状態を変更する。
- 1つの魔法の直接効果、MP消費、エーテル注入、対属性反作用を一つの確定処理として扱う。
- 失敗時にHP、MP、エーテル、クールダウンを変更しない。
- 同一frameの入力をsequence順に処理し、途中失敗で不正な部分適用を残さない。
- `combatResult`／`spellResult`に実効威力、ダメージ、残HP、環境変化、inputSequenceを記録する。
- request IDまたは入力sequenceで再送・二重実行を排除する。
- 結果EventはWorldUpdate sequence順に配信する。

## 未確定事項

- MPコストとクールダウンのデータモデル
- Player／NPC／対象物共通のCombatTarget interface
- 属性耐性とFFXI型8属性モデル移行後の優位倍率
- 失敗CommandをクライアントへEvent化するか応答だけにするか

これらはLISS-0134、NPC、クライアント通信の仕様と矛盾しない形で確定する。

---

## English

Separate attack/spell input acceptance from authoritative resolution. Validate all actors, range, power, elements, resources, and cooldowns before changing state. Apply direct effects, resource cost, ether changes, and cooldown atomically, then publish deterministic result Events with effective power, damage, remaining HP, environment deltas, and input sequence.

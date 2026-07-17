# LISS-0126: 権威サーバー攻撃・魔法

- Status: proposed
- Priority: high
- Depends on: LISS-0113, LISS-0115, LISS-0120

## 目的

攻撃と魔法をWorldInputとして受け、対象、距離、威力、クールダウン、環境エーテル変化を決定論的に処理する。

## 受入条件

- 攻撃者・対象をサーバー状態から解決する。
- 威力、射程、属性、クールダウン、対象種別を検証する。
- 結果とエーテル変化を同じWorld更新系列へ載せる。
- 不正入力、二重送信、切断中入力でHPや環境を破壊しない。
- hazardの対象物への影響は仕様化してから適用する。

## English

Resolve attacks and spells authoritatively as WorldInputs, validating actors, range, power, element, cooldown, and target type. Replicate results and ether changes in one deterministic update stream.

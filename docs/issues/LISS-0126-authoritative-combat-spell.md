# LISS-0126: 権威サーバー攻撃・魔法

- Status: review
- Priority: high
- Depends on: LISS-0113, LISS-0115, LISS-0120

環境エーテルの属性・影響順序・対属性は
[`docs/architecture/ether-rules.md`](../architecture/ether-rules.md)を参照する。

## 目的

攻撃と魔法をWorldInputとして受け、対象、距離、威力、クールダウン、環境エーテル変化を決定論的に処理する。

## 受入条件

- 攻撃者・対象をサーバー状態から解決する。
- 威力、射程、属性、クールダウン、対象種別を検証する。
- 結果とエーテル変化を同じWorld更新系列へ載せる。
- 不正入力、二重送信、切断中入力でHPや環境を破壊しない。
- hazardの対象物への影響は仕様化してから適用する。
- 成功した魔法だけがHP／MP／エーテルを変更する。
- 伝導率計算は発動前の環境値を一度だけ参照する。
- エーテル注入、対属性反作用、実効威力を結果Eventへ記録する。

## 実装資料

`ServerCommandDispatcher`へAttack／CastSpellを接続した。アクティブな匿名セッションだけがCombatCommandHandlerを通じてWorldInputQueueへ投入でき、クライアント申告の別内部IDは拒否する。結果Eventの基礎はLISS-0137で追加した。完全な原子性、クールダウン、MP、NPC対象は後続スライスで扱う。テスト・ビルドは実行していない。

## English

Resolve attacks and spells authoritatively as WorldInputs, validating actors, range, power, element, cooldown, and target type. Replicate results and ether changes in one deterministic update stream.

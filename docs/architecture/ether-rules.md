# 環境エーテル属性・相互作用ルール

Status: v1 deterministic simulation profile

## 目的

この文書は、プレイヤーの魔法・攻撃と環境エーテルの関係を、サーバー権威の決定論的なWorld更新として定義する。クライアントは結果を計算せず、Snapshot／Eventを表示する。

## 属性

初期実装で扱う属性は次の4種類とする。

| 属性 | 役割の暫定的意味 | 対属性 |
| --- | --- | --- |
| fire | 熱・燃焼・攻撃的な変化 | water |
| water | 冷却・流動・鎮静的な変化 | fire |
| earth | 固定・物質・防御的な変化 | air |
| air | 流動・拡散・変位的な変化 | earth |

「役割の意味」はゲーム表現の初期分類であり、具体的なPlayer／NPC／地形への効果は別の受入仕様で確定する。`scode`資料に登場する`soil`は`earth`、`wind`は`air`へ対応付ける。

## エーテル値

各属性はWorldごとに符号付き値を持つ。

- 初期値：0
- 範囲：-100000〜100000
- 正の値：当該属性の蓄積・濃化
- 負の値：当該属性の枯渇・反転方向の偏り

正負どちらの偏りも伝導率と不安定度へ影響する。負の値を「無効」や「ゼロ」として扱わない。

## 魔法の影響順序

魔法1件は、同じWorld frame内で次の順序に固定する。

1. 発動者・対象・射程・属性・威力・リソースを検証する。
2. 発動前の使用属性値から伝導率を計算する。
3. 実効威力を計算する。
4. 対象への直接効果を適用する。
5. 使用属性へ環境影響を注入する。
6. 対属性へ反作用を適用する。
7. 結果と環境変化を同一のWorldUpdate系列へ記録する。
8. フレーム内の全入力処理後に全属性を減衰する。

直接効果の適用に失敗した魔法は、HP・MP・エーテルを変更しない。複数入力で途中失敗しても、既に確定した別入力を巻き戻すか、フレーム開始時に全件検証して部分適用を防ぐ。採用方式はLISS-0126で固定する。

## 伝導率

v1の実装式は次のとおり。

```text
conductivity[a] = 1.0 + min(abs(ether[a]) / 100.0, 1.0)
```

したがって伝導率は1.0〜2.0となる。エーテルの絶対値が大きいほど、同属性の魔法の実効威力が上がる。地理的な近傍伝播へ同じ係数を二重適用しない。

## 属性相互作用

v1では対属性だけを定義する。

```text
fire  ↔ water
earth ↔ air
```

基礎威力を`P`としたとき、環境注入量は次のとおり。

```text
influence = P × 0.1
usedAttribute += influence
opposingAttribute -= influence × 0.25
```

これは「対属性を直接攻撃する」意味ではなく、環境バランスを崩す反作用である。

v1では、fire→water→earth→air→fireの循環伝播や6方向グリッド拡散はまだ採用しない。これらは`WorldGrid`導入時に別途仕様化し、現在の4属性対属性モデルと混在させない。

## 不安定度とhazard

```text
instability = min((abs(fire) + abs(water) + abs(earth) + abs(air)) / 200.0, 10.0)
hazard = max(0.0, instability - 1.0)
```

hazardは環境状態の観測値であり、v1では自動的にPlayer／NPCへダメージを与えない。対象への副作用は、対象種別、範囲、持続時間、耐性、回復方法を定義した後に別Issueで実装する。

## 減衰

20FPSのWorld frameで全入力を処理した後、各属性へ次を適用する。

```text
ether[a] *= 0.98
```

注入・相互作用・hazard算出・減衰の順序を変更しない。Snapshotには減衰後の値を掲載する。

## 未確定事項

- 属性ごとの具体的なゲーム効果
- Player／NPC／地形の属性耐性
- MP・クールダウン・コスト
- 近傍セルへの伝播と地域ごとのEtherField
- hazardの対象影響
- エーテルの永続化と再起動時復旧

これらはLISS-0126、LISS-0127、LISS-0133およびWorldGrid関連Issueで確定する。

## FFXI型要素関係の移行候補

先行ゲームの設計知を利用する候補として、FFXIで知られる次の関係を採用候補にする。

```text
fire → ice → wind → earth → thunder → water → fire
light ↔ dark
```

この関係は、6属性の循環優位と光／闇の対立を持つため、現在の4属性対属性モデルよりも、弱点、耐性、環境伝播、魔法連携を整理しやすい。ただし、これはFFXIの固有コンテンツや数値を複製するものではなく、本プロジェクトの環境エーテルシミュレーションへ抽象化して取り込む候補である。

移行時は次を分離して検討する。

- `earth`と`wind`は現行`earth`／`air`との互換・改名方針を決める。
- `ice`と`thunder`を新属性として追加する。
- `light`と`dark`を通常の循環属性ではなく対立軸として扱う。
- 優位関係を直接ダメージ倍率にするか、伝導率・耐性・環境反応へ分配する。
- 既存Snapshotの4属性payloadをバージョン更新する。
- 旧4属性Worldを8属性Worldへ移行する決定的な変換式を定義する。

この移行候補の採用判断と実装はLISS-0134で行う。LISS-0134が完了するまでは、v1の4属性モデルを実装上の正とする。

---

## English summary

The v1 deterministic profile uses four signed ether attributes: fire, water, earth, and air. Fire opposes water, and earth opposes air. An FFXI-inspired six-element wheel plus light/dark opposition is a migration candidate tracked by LISS-0134; until it is approved, the four-element profile remains authoritative. Spell conductivity is `1 + min(abs(ether) / 100, 1)`. A successful spell adds `basePower * 0.1` to its attribute and subtracts 25% of that influence from the opposing attribute. All inputs are processed in sequence order, then ether decays by `0.98` once per 20 FPS world frame. Hazard is an observable environment value only; concrete effects on players, NPCs, or terrain require a separate approved specification.

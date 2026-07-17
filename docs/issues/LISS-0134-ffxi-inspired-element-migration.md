# LISS-0134: FFXI型エーテル属性関係への移行検討

- Status: proposed
- Phase: phase-0-design
- Type: architecture + deterministic simulation
- Priority: high
- Depends on: LISS-0114, LISS-0115, LISS-0126, LISS-0129
- Blocks: LISS-0135, LISS-0136

## 背景

FFXIで知られる`fire → ice → wind → earth → thunder → water → fire`の循環と、`light ↔ dark`の対立は、属性の優位関係、耐性、連携、環境反応を整理する出発点として利用しやすい。これはゲーム固有の数値・名称・コンテンツを複製するのではなく、関係モデルを本プロジェクトへ抽象化する検討である。

## 目的

現行の`fire / water / earth / air`と`fire↔water`、`earth↔air`のv1モデルを、8属性モデルへ移行する価値と安全な変換方法を決める。

## 候補モデル

```text
fire → ice → wind → earth → thunder → water → fire
light ↔ dark
```

矢印は「優位関係」の候補であり、必ずしも直接ダメージ倍率を意味しない。優位関係を、伝導率、対象耐性、環境反作用、魔法連携のどこへ割り当てるかを別々に評価する。

## 受入条件

- 6属性循環とlight／dark対立の意味を用語として定義する。
- 現行`air`を`wind`へ改名するか、互換aliasとして残すか決める。
- 現行4属性Snapshotを8属性Snapshotへ移行する変換式を決める。
- 既存の環境値、伝導率、減衰、hazard、WorldUpdate sequenceを壊さない移行手順を作る。
- 旧クライアントが未知属性を受け取った場合の拒否・互換動作を決める。
- FFXI固有の数値、コンテンツ、名称、著作物をコピーしない。
- 移行を実装する前にADRで採用可否を記録する。

## 採用しない場合の代替

4属性モデルを継続し、`air`を風として文書上の表示名だけ`wind`へ寄せる。属性追加を行わず、対属性・伝導率・hazardのルールを安定化する。

## 次のIssue

- LISS-0135: 8属性Snapshot／Protocol migration
- LISS-0136: 属性優位・耐性・環境反応のルール確定

---

## English

### Purpose

Evaluate an FFXI-inspired relationship model: `fire → ice → wind → earth → thunder → water → fire` with `light ↔ dark` opposition. Abstract the relationship model without copying game-specific numbers, content, names, or copyrighted assets.

### Acceptance criteria

- Define the six-element wheel and light/dark opposition.
- Decide whether current `air` becomes `wind` or remains a compatibility alias.
- Define deterministic migration from the four-element Snapshot to eight elements.
- Preserve ether, conductivity, decay, hazard, and WorldUpdate sequencing.
- Define behavior for old clients receiving unknown attributes.
- Record adoption or rejection in an ADR before implementation.

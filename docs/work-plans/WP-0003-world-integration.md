# WP-0003: scode world の C++ 統合計画

## Scope

LISS-0018 に対応する計画。`scode` の world 仕様を `seed-origin` の一般的なC++構成へ取り込む。

## Plan

| Order | Work item | Status | Output |
| --- | --- | --- | --- |
| 1 | `world-model` の型・距離・制限判定を定義 | pending | C++ model/API |
| 2 | `world-grid` の生成・循環・拡散を実装 | pending | grid component and tests |
| 3 | `world-effects` の寿命と適用を実装 | pending | effect processing and tests |
| 4 | `world-actions` のキュー・管理操作・コマンドレジストリを実装 | pending | action processing and tests |
| 5 | `world-runtime` の開始・停止・周期処理を実装 | pending | runtime service and tests |
| 6 | 通信・永続化の必要性を再評価 | pending | separate follow-up issue or explicit omission |

## Design constraints

- 既存の `Field` を直接 `World` 化せず、プレイヤー状態とワールド進行を分離する。
- 初期実装は標準C++と既存プロジェクトで完結させる。
- ワーカースレッドは停止可能で、開始・停止を繰り返しても増殖しないようにする。
- グリッドとプレイヤーの状態はテストごとにリセットできるようにする。
- 未知コマンド、空キュー、欠損対象、境界グリッドをエラーとして安全に扱う。

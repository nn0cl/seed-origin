# WP-0003: scode world の C++ 統合計画

## Scope

LISS-0018 に対応する計画。`scode` の world 仕様を `seed-origin` の一般的なC++構成へ取り込む。

## Goal

プレイヤー、対象物、魔法・攻撃、3次元環境エーテル、減衰、伝導率、属性相互作用を持つ権威型ワールドシミュレーションを、既存C++コードの安全性を確認した後に段階的に構築する。

## Execution Gate

LISS-0019が完了し、AdjudicatorがPhase 1開始を承認するまで、実装、コンパイル、テスト実行、移行は行わない。今回の計画ではテストケースの仕様と配置だけを定義し、テストの実行結果は作成しない。

## Plan

| Order | Work item | Status | Output |
| --- | --- | --- | --- |
| 1 | LISS-0019: 移行安全性ゲートと所有権契約 | in_progress | safety review, ownership design, no code execution |
| 2 | LISS-0020: エンティティ・座標・対象物モデル | pending | reviewed spec, Phase 1 test artifacts |
| 3 | LISS-0021: EtherFieldのデータ構造・減衰・伝導率 | pending | field spec, performance model, Phase 1 test artifacts |
| 4 | LISS-0022: 決定論的イベント・効果解決 | pending | event/effect spec, Phase 1 test artifacts |
| 5 | LISS-0023: World runtimeと時間進行 | pending | lifecycle spec, Phase 1 test artifacts |
| 6 | LISS-0024: 通信境界・スナップショット・運用受入条件 | pending | adapter boundary and operational acceptance spec |
| 7 | 子IssueごとのPhase 1テストレビュー | pending | reviewed tests; execution intentionally deferred |
| 8 | 子IssueごとのPhase 2以降の実装・検証 | blocked | only after safety gate and phase approvals |

## Design constraints

- 既存の `Field` を直接 `World` 化せず、プレイヤー状態とワールド進行を分離する。
- シミュレーションコアを権威状態の唯一の更新元とし、通信・AI推論は外側のアダプターに分離する。
- 初期実装は標準C++と既存プロジェクトで完結させる。
- ワーカースレッドは停止可能で、開始・停止を繰り返しても増殖しないようにする。
- グリッドとプレイヤーの状態はテストごとにリセットできるようにする。
- 未知コマンド、空キュー、欠損対象、境界グリッドをエラーとして安全に扱う。
- 全セル更新とアクティブセル更新を比較し、更新周期を固定値として埋め込まない。
- エーテル濃度の基準値からの変化に応じて属性伝導率が増加することを検証する。
- 魔法の対象効果と環境エーテルへの影響を別イベントとして検証する。
- 同じワールド状態と入力イベント列から同じ魔法結果が得られ、環境変化後には結果が変わることを検証する。
- 属性バランスの崩れによる二次イベントを、イベント連番順に再現できることを検証する。

## Issue Graph

| Issue | Depends on | Blocks | Size | Branch |
| --- | --- | --- | --- | --- |
| LISS-0019 | LISS-0017 | LISS-0020〜LISS-0024 | L | `main` |
| LISS-0020 | LISS-0019 | LISS-0021, LISS-0022 | L | `main` |
| LISS-0021 | LISS-0019, LISS-0020 | LISS-0022, LISS-0023 | L | `main` |
| LISS-0022 | LISS-0020, LISS-0021 | LISS-0023 | L | `main` |
| LISS-0023 | LISS-0022 | LISS-0024 | M | `main` |
| LISS-0024 | LISS-0023 | 統合判断 | M | `main` |

## Bug handling policy

実装時に発見したバグは、修正前に次の順序で記録する。

1. 発生箇所、再現条件、影響範囲、所有権・決定論・性能への影響をIssueまたは本計画へ記録する。
2. 既存の受入仕様に含まれるか、仕様未確定か、現在のIssue範囲外かを分類する。
3. メモリ破壊、二重解放、データ競合、決定論の破壊、外部契約破壊は必ず個別Issue化し、修正前にレビューする。
4. サイズSで、承認済みスコープ内、期待動作が明確、単一領域、境界変更なし、決定的検証あり、一回で完了の条件をすべて満たす局所バグだけFast Pathを許可する。
5. Fast Pathを使った場合も、Issueまたは計画へ `Minor bug; fixed within approved scope; separate plan not required` と記録する。

メモリ安全性・データ破壊・決定論を壊すバグは発見時点で当該Issueを停止し、先に修正方針を吟味する。それ以外は現在の受入条件を阻害するか、後続Issueの前提を壊すかで修正タイミングを決める。

## TDD handling for this planning phase

- 各子IssueはPhase 1でGherkin/EARSと失敗テストを設計・実装する。
- テストは実装内容を理解する資料として、入力、状態、イベント順、期待結果、所有権境界を明示する。
- 今回の計画・設計段階ではテストを実行しない。Red/Greenの実測結果も作成しない。
- Phase 1テストのレビューとAdjudicator承認後にのみPhase 2実装へ進む。

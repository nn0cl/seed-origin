# world 実装仕様と seed-origin 取り込み方針

## 調査対象

対象リポジトリは `/Users/nn0cl/Documents/git/scode`、基準コミットは次のとおりです。

- `f6053c5`（2022-07-15）: `World` と `Grid` の環境更新
- `22a1ffc`（2022-07-15）: world 関連モデル・サービスの追加と環境更新
- `a447aa7`（2015-10-26）: パッケージ名変更

world の中心実装は次のファイルです。

- `src/main/java/jp/co/dstechnology/scode/service/World.java`
- `src/main/java/jp/co/dstechnology/scode/service/WorldStatus.java`
- `src/main/java/jp/co/dstechnology/scode/service/Environment.java`
- `src/main/java/jp/co/dstechnology/scode/model/Grid.java`
- `src/main/java/jp/co/dstechnology/scode/model/Action.java`
- `src/main/java/jp/co/dstechnology/scode/model/ActiveEffect.java`
- `src/main/java/jp/co/dstechnology/scode/model/Command.java`
- `src/main/java/jp/co/dstechnology/scode/model/Player.java`
- `src/main/java/jp/co/dstechnology/scode/model/Status.java`
- `src/main/java/jp/co/dstechnology/scode/model/Element.java`

## 仕様の要約

### ワールドライフサイクル

`World` はシングルトンとして取得され、初期化時に次を行います。

1. `WorldStatus` を生成する。
2. 座標 `-10` 以上 `10` 未満の各軸を持つ、20 × 20 × 20 のグリッドを生成する。
3. 10ミリ秒間隔のワーカースレッドを開始する。
4. 各周期でグリッド環境を循環させ、最大60件のアクションを処理する。

設定ファイル `scode.ini` には `time_span=10` があるものの、現行 `World` は値を読まず、10ミリ秒を直接使用しています。取り込み時は設定値を正式な周期として扱う方針です。

### グリッド環境

各 `Grid` は次を保持します。

- 3次元座標
- 基準元素量
- `fire` / `water` / `soil` / `wind` の元素量

`circulate()` は、元素量の偏りを次の順で隣接元素へ移します。

`fire → water → soil → wind → fire`

総元素量が基準値を超える場合は、6方向の隣接グリッドへ元素を拡散します。既存C++の `Field` はプレイヤーとアクションキューを担当しているため、グリッド環境は `WorldGrid` 相当の独立コンポーネントとして追加するのが安全です。

### プレイヤーと効果

プレイヤーはIDをキーにワールドへ登録されます。ログイン時は追加、ログアウト時は削除します。アクションが成立すると、対象プレイヤーへ `ActiveEffect` を追加します。

効果は次の情報を持ちます。

- 残り寿命
- 付与する `Status`

周期処理では効果を適用し、寿命を減らし、寿命切れの効果を削除します。これは既存C++の `Buff` と近い責務ですが、寿命と周期処理を明示的に持たせる必要があります。

### アクション実行条件

通常アクションは `ActionType.ACTION` の場合にキューへ入り、管理アクションは即時処理されます。

通常アクションの実行条件は次のとおりです。

1. コマンドIDからコマンド定義を取得できる。
2. 実行者のHP・MP・元素量がコマンド制限以上である。
3. 実行者と対象者の距離がコマンドの距離制限以内である。
4. 条件を満たした場合、対象者へコマンドの効果を付与する。

距離は3次元ユークリッド距離とします。

`Command` は少なくとも次を持ちます。

- ID、名称
- 距離制限
- 実行制限 `Status`
- 付与値 `Status`
- 効果寿命
- 詠唱時間（現行実装では未使用）

### ワールド状態

`WorldStatus` は次の監視値をJSON化できます。

- `ucount`: 現在のプレイヤー数
- `average`: 平均値用の未実装フィールド
- `uptime`: プロセス起動からの経過時間
- `instances`: インスタンス数用の未実装フィールド

`average` と `instances` は仕様未確定のため、C++移植の初期段階では予約フィールドとして保持します。

## seed-origin への対応づけ

| scode | seed-origin | 取り込み方針 |
| --- | --- | --- |
| `World` | `Field` + 新規 `World` 管理クラス | `Field` の既存キューを壊さず、周期処理を新規サービスへ分離 |
| `Grid` | 新規 `WorldGrid` / `ElementGrid` | `Position` をキーにした3次元グリッド管理として追加 |
| `Player` | `Player` | 既存のID、状態、位置、バフを利用し、効果寿命を拡張 |
| `Position` | `Position` | 距離計算と座標キーの比較契約を追加 |
| `Status` / `Element` | `Status` + 新規 `Element` | HP/MPと元素値を分離し、制限比較を共通化 |
| `Action` | `Action` | 通常・管理アクション種別、実行者、対象者、コマンドIDを整理 |
| `ActiveEffect` | `Buff` | 寿命、効果適用、期限切れ削除を追加 |
| `Command` / `CommandMaster` | 新規 `Command` / `CommandRegistry` | 初期はインメモリ定義。DB/MyBatisは移植対象外 |
| `WorldStatus` | 新規 `WorldStatus` | C++の値型とJSON/ログ出力を設計 |
| `Environment` | `WorldGrid` の周期処理 | 空の `Environment::affect` は直接移植しない |

## 取り込み時に修正すべき既知不具合

world の現行実装には、仕様として採用する前に修正が必要な点があります。

- `World.addPlayer` と `setRunningStatus` が常に `false` を返す。
- `World.getPlayer(String id, String hash)` が `id` を使わず `hash` だけで検索する。
- `World.spendTime` はアクションが空でも `pop()` するため例外になる。
- アクションキューが周期ごとに最大60件ではなく、空キュー時の安全性がない。
- `distance` が差分の合計に対して平方根を取っており、ユークリッド距離になっていない。
- `CommandMaster.get` が自身を再帰呼び出ししている。
- `Player.process` が拡張for中にリスト要素を削除する。
- `Player.affect` が対象プレイヤーのステータスではなく、効果側のステータスを変更している。
- `World.raiseWorld` が再初期化時にグリッドを明示的にクリアしない。
- `World` の静的状態とワーカースレッドのライフサイクルが安全に分離されていない。
- `Grid` の静的マップがテスト間で共有され、リセットAPIがない。
- `Grid.affect` は対象グリッドがない場合にnull参照になる。
- `Element.Builder.fromJson` が `null` を返す。
- Javaコンパイラ設定は `<java.version>17</java.version>` としつつ、source/targetは8で不一致。

これらはC++への移植時に再現せず、受入条件に含めます。

## 取り込み単位

履歴と責務が衝突するため、既存の `seed-origin` のC++ソースへ直接上書きせず、次の単位で段階的に取り込みます。

1. `world-model`: `Element`、`WorldStatus`、コマンド定義、座標距離計算。
2. `world-grid`: グリッド生成、循環、6方向拡散、リセット可能な保管領域。
3. `world-effects`: `Buff` の寿命処理とプレイヤーへの安全な効果適用。
4. `world-actions`: 通常・管理アクション、制限判定、距離判定、アクションキュー。
5. `world-runtime`: 周期スレッド、開始・停止、ワールド状態集計。

最初の統合ではDB、MyBatis、JAX-RS、JSON通信を含めず、決定的なインメモリ処理として実装します。通信層はワールドコアの受入テスト後に別Issueとして扱います。

## 受入テストの骨子

- 既定サイズのグリッドが一度だけ生成され、リセット後に同じ状態へ戻る。
- 元素循環と6方向拡散が境界条件を含めて決定的に動作する。
- 既存プレイヤーをIDで追加・更新・削除できる。
- 管理アクションのログイン・ログアウトが安全に再実行できる。
- コマンド制限未達、距離超過、未知コマンドが副作用を起こさない。
- 成功したアクションだけが対象プレイヤーへ効果を付与する。
- 効果寿命が周期ごとに減少し、期限切れが安全に削除される。
- ワールドの開始・停止を複数回行ってもスレッドが増殖しない。
- 空のアクションキューを周期処理してもクラッシュしない。

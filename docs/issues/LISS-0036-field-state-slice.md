# LISS-0036: Fieldの状態更新・プレイヤー管理スライス

## Metadata

- Local issue ID: LISS-0036
- Parent issue: LISS-0026
- Status: review
- Phase: phase-1-red
- Type: correctness + safety
- Priority: high
- Related branch: `main`

## Scope

FieldのプレイヤーID索引、退出処理、位置キュー、Actionキューの最低限の状態更新を実装する。World固有のエーテル計算やスレッドセーフなキューは対象外とする。

## Acceptance criteria

- プレイヤーIDを`int64_t`のまま保持し、暗黙の縮小変換を行わない。
- `unsetPlayer`が対象を削除し、対象が存在しない場合は失敗を返す。
- Field対象、Player対象、Self対象のActionが、対応する状態へ反映される。
- 位置・Actionキューは1回の`processFrame`で決定的な順序で消費される。
- 空キューを安全に処理する。
- 状態更新と退出処理のテストコードを追加する（実行はしない）。

## Verification note

実装とテスト資料の追加まで完了。運用規約によりコンパイル・テストは実行していない。CodeQLも既知不都合修正後の設定方針のため、今回のプッシュ時点ではレビューなし。

## Out of scope

- エーテル属性、減衰、伝導率、距離計算。
- 複数スレッドからのキュー投入。

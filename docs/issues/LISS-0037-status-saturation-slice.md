# LISS-0037: Status加算の飽和・オーバーフロー安全化

## Metadata

- Local issue ID: LISS-0037
- Parent issue: LISS-0027
- Status: review
- Phase: phase-1-red
- Type: safety + correctness
- Priority: high
- Related branch: `main`

## Scope

HP/MPの加算・減算で符号付き整数オーバーフローを起こさず、値域`[0, LONG_MAX]`へ飽和させる。

## Acceptance criteria

- 正の加算が`LONG_MAX`を超える場合は`LONG_MAX`になる。
- 負の加算で0を下回る場合は0になる。
- `LONG_MIN`/`LONG_MAX`を含む入力で未定義動作を起こさない。
- HP/MPの仕様と境界値をテストコードで固定する（実行はしない）。

## Deferred findings

Buffの開始時刻・寿命・周期適用は別スライスで扱う。現時点で仕様を推測して`processBuffs`の適用頻度を変更しない。

## Verification note

実装とテスト資料の追加まで完了。コンパイル・テストは実行していない。CodeQLは既知不都合修正後に設定する方針のため、今回のプッシュ時点ではレビューなし。

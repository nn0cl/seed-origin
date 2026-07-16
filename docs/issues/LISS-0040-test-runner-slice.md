# LISS-0040: CMakeテストランナー基盤スライス

## Metadata

- Local issue ID: LISS-0040
- Parent issue: LISS-0030
- Status: review
- Phase: phase-1-red
- Type: test + build
- Priority: high
- Related branch: `main`

## Scope

Xcodeの単一実行ターゲットに依存せず、CMake/CTestで本体ライブラリ、CLI、テストランナーを分離できる最小基盤を追加する。既存のテスト資料は新ランナーから呼べる形にする。

## Acceptance criteria

- 本体コードを`seed_core`ライブラリとして定義する。
- CLIとテスト実行ファイルを本体から分離する。
- 現在の安全性スライスのテスト関数をCTestから呼び出せる。
- `tests/legacy`と旧空テストを新しい実行対象へ混入させない。
- 実行方法を文書化する。

## Verification note

CMake設定とテストランナーは追加するが、既存の運用規約によりconfigure/build/testは実行しない。

実装と構成確認まで完了。CodeQLは前回レビューで未解決アラートなし。

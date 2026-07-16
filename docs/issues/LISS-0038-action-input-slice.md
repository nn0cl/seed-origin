# LISS-0038: Action入力検証スライス

## Metadata

- Local issue ID: LISS-0038
- Parent issue: LISS-0028
- Status: review
- Phase: phase-1-red
- Type: safety + correctness
- Priority: high
- Related branch: `main`

## Scope

Action生成時に種別と必要なPlayer参照を検証し、null入力や未知の種別をデフォルトPlayerとして実行しない。

## Acceptance criteria

- Field Action（type 0）はPlayerなしで生成できる。
- Player対象 Action（type 1）は実行者・対象者が必須。
- Self Action（type 2/3）は実行者が必須。
- 未知のAction種別は無効としてFieldで無視される。
- 無効Actionを検出できるAPIとテスト資料を追加する（実行はしない）。

## Deferred findings

Actionの結果型、射程、エーテル効果、変更不能getterは別スライスで扱う。

## Verification note

実装とテスト資料の追加まで完了。コンパイル・テストは実行していない。CodeQLは既知不都合修正後に設定する方針のため、今回のプッシュ時点ではレビューなし。

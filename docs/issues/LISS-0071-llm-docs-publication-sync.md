# LISS-0071: LLM連携文書の公開同期

## Metadata

- Local issue ID: LISS-0071
- Status: review
- Phase: phase-3-refactor
- Type: process + documentation
- Priority: high
- Related commit: `1146c0c`
- Related: LISS-0030, LISS-0031

## Finding

テンプレート導入で生成された`docs/`、`.github/`、`.cursor/`、`.grok/`、`CLAUDE.md`、スクリプトがローカルには存在したが、未追跡のままコミットされていなかった。そのため、GitHub上の他メンバーからLLM連携契約を参照できなかった。

## Resolution

解決済み。`1146c0c`でLLM連携文書・ADR・テンプレート・ルール・CI・スクリプト計82ファイルをmainへ追加し、origin/mainへプッシュした。

## Acceptance

- `AGENTS.md`が参照する文書がGitHub上で取得できる。
- LLM連携ルール、Issue運用、CodeQL、テスト方針を他メンバーが参照できる。
- 文書追加後のCodeQLレビューで未解決アラートがない。
- 今後のテンプレート導入・更新時に未追跡ファイルを見落とさない確認手順を残す。

## Verification

- CodeQL run `29532312872`: success
- Open CodeQL alerts: 0
- Resolution commit: `1146c0c`
- Test/build execution: not run by project policy

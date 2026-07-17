# Contributing

## 基本原則

本リポジトリでは、人間とAIエージェントを同じ開発規範の対象として扱います。主体の種類ではなく、現在担っている役割と意思決定権を区別します。人間もAdjudicatorではないDeveloperとして参加できます。

- **人間のAdjudicator（審判者）**: フェーズ移行、Phase 1テスト、ADR、曖昧な仕様、外部サービスやライセンスの採否を判断する。
- **Developer（人間またはAIエージェント）**: 設計 intake、最小コンテキスト選択、仕様に沿った成果物作成、検証、前提と未決事項の記録を行う。自分の成果物を自分の判断だけで承認しない。
- **決定論的ツール**: テスト、静的解析、CodeQL、フォーマッターなど、モデル判断に依存しない検証信号を提供する。

すべてのDeveloperは同じ運用規範、安全性基準、受入条件に従います。Developerとして参加する人間は、AIエージェントと同じく、承認された範囲で作業し、Adjudicatorの承認権限を持ちません。

作業開始前に、少なくとも以下を確認してください。

- [AGENTS.md](AGENTS.md)
- [プロジェクト構成](docs/architecture/project-structure.md)
- [実装準備条件](docs/architecture/implementation-readiness.md)
- [Issue計画規約](docs/collaboration/local-issue-planning.md)
- [完了定義](docs/collaboration/definition-of-done.md)
- [LICENSE](LICENSE)

## Issueと実装

1. 目的、仕様、受入条件、依存関係をIssueに記録する。
2. 必要に応じて作業計画とスライスIssueを追加する。
3. 受入条件を満たす最小単位で実装する。
4. テスト資料を追加する。テスト実行の可否は運用規約に従う。
5. ドキュメント、Issue、作業計画を実装と同期する。
6. Issue完了時にコミットし、mainへプッシュする。
7. プッシュ後にCodeQLの結果を確認し、指摘があれば最優先Issueとして修正する。

## コードの安全性

- 未検証の外部入力をWorldやファイル処理へ渡さない。
- 所有権を明確にし、raw pointerによる所有を避ける。
- 境界、サイズ、整数変換、socketの切断・部分readを検証する。
- サーバー権威値をクライアント入力で上書きしない。
- 秘密情報、個人情報、脆弱性の詳細をコミットしない。

## Pull Requestとコミット

コミットまたはPull Requestには、目的、変更範囲、受入条件、未実行の検証、関連Issueを記載してください。無関係な整形やファイル移動を同じ変更へ混ぜないでください。

## 貢献物の許諾

貢献者は、提出するコード・文書・アセットについて必要な権利を持ち、第三者の権利を侵害しないことを確認してください。貢献を提出した時点で、貢献者はリポジトリ所有者に対し、貢献を本プロジェクトへ組み込み、変更、複製、配布するための無期限・全世界的・無償の許諾を与えるものとします。

この許諾は、本リポジトリの[カスタムライセンス](LICENSE)を変更しません。第三者が改変物・派生物を作成・配布する権利を自動的に与えるものでもありません。条件に同意できない場合は、コードや文書を提出しないでください。

## 行動規範と脆弱性

行動規範は[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md)、脆弱性の報告は[SECURITY.md](SECURITY.md)を参照してください。

---

# English version

The Japanese version is authoritative. This section is a translation; if the
two versions conflict, the Japanese version prevails.

## Roles

- **Adjudicator**: A human decision-maker who approves phase transitions,
  Phase 1 tests, ADRs, ambiguous requirements, and external service or license
  choices.
- **Developer**: A human or AI agent who performs design intake, context
  selection, implementation, verification, and evidence recording. Developers
  do not approve their own work.
- **Deterministic tools**: Tests, static analysis, CodeQL, and formatters that
  provide repeatable verification signals without making product decisions.

Human Developers and AI Developers follow the same operating rules, safety
requirements, acceptance criteria, and review gates. Being human does not grant
Adjudicator authority.

## Before starting work

Read [AGENTS.md](AGENTS.md), the project structure, implementation readiness,
local issue planning, definition of done, and [LICENSE](LICENSE).

## Issue and implementation workflow

1. Record the goal, specification, acceptance criteria, and dependencies in an Issue.
2. Add a work plan or slice Issue when needed.
3. Implement the smallest unit that satisfies the acceptance criteria.
4. Add test artifacts; follow the operating rule for whether tests may run.
5. Synchronize documentation, Issues, and work plans with the implementation.
6. Commit the completed Issue and push `main`.
7. Inspect CodeQL after the push. Findings become the highest-priority Issue.

## Safety

- Do not pass unvalidated external input to the World or file processing.
- Make ownership explicit and avoid owning raw pointers.
- Validate boundaries, sizes, integer conversions, socket closure, and partial reads.
- Do not let client input overwrite server-authoritative state.
- Never commit secrets, personal data, or undisclosed vulnerability details.

## Contribution permission

Contributors must have the rights required for the code, documentation, or
assets they submit. By submitting a contribution, a contributor grants the
repository owner an irrevocable, worldwide, royalty-free permission to
incorporate, modify, reproduce, and distribute that contribution as part of
the project. This does not change the [custom license](LICENSE) or grant third
parties permission to create derivative works.

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for conduct rules and
[SECURITY.md](SECURITY.md) for vulnerability reporting.

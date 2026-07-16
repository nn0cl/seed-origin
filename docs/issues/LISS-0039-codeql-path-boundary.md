# LISS-0039: CodeQLパスインジェクション修正

## Metadata

- Local issue ID: LISS-0039
- Source: CodeQL alerts #1 and #2
- Status: in_progress
- Phase: phase-2-codeql-fix
- Type: security
- Priority: critical
- Related branch: `main`

## Finding

CodeQL `cpp/path-injection` が、コマンドライン引数から渡されたパスを`std::ifstream`/`std::ofstream`へ直接渡している箇所を検出した。

## Scope

- 既存入力ファイルは`realpath`で存在確認と正規化を行う。
- 出力先は親ディレクトリを`realpath`で正規化し、ファイル名を分離して再構成する。
- 正規化に失敗したパスはI/O前に拒否する。
- 出力ファイル名を安全文字のallowlistで検証する。
- CodeQL再実行でアラートが解消されることを確認する。

## Verification

CodeQL実行後にアラート状態を確認する。コンパイル・テストは既存の運用規約により実行しない。

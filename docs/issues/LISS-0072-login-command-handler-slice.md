# LISS-0072: LoginCommand→SessionRegistry接続スライス

- Status: review
- Phase: phase-1-red
- Type: feature + security
- Priority: high
- Parent: LISS-0057
- Depends on: LISS-0053, LISS-0070

## Scope

検証済みのLogin CommandをSessionRegistryへ渡し、仮セッション結果または拒否理由を返すサーバー側アプリケーション境界を追加する。socket accept、暗号化、永続化、World参加は後続Issueで扱う。

## Acceptance criteria

- Login以外のCommandを拒否する。
- Command検証失敗をSessionRegistryへ渡さない。
- 内部IDをクライアント入力から採用しない。
- 成功時に仮セッション情報を返し、認証済みとは表示しない。
- 申告IDの名寄せはSessionRegistryの契約に従う。

実装とテスト資料を追加した。socket接続、World参加、永続化、認証は未実装。テスト・ビルドは実行していない。

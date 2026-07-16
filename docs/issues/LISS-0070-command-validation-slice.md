# LISS-0070: 通信Command検証スライス

- Status: review
- Phase: phase-1-red
- Type: security + architecture
- Priority: critical
- Parent: LISS-0044
- Related: LISS-0055, LISS-0058

## Scope

通信層とWorldの間に、バージョン・種別・セッションID・payload長を検証する値型境界を追加する。バイト列のdecode、暗号化、socket read/writeは後続スライスに残す。

## Acceptance criteria

- 未知のCommand種別を拒否する。
- ログイン以外で正のセッション内部IDを要求する。
- ログインでクライアント指定の内部IDを受け付けない。
- プロトコルバージョンとpayload最大長を検証する。
- 検証失敗をWorldへ渡さない。

実装とテスト資料を追加した。socketのframe decode・暗号化・送受信は後続Issueで扱う。テストは実行していない。

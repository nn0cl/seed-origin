# LISS-0074: Protocol v1フレームcodecスライス

- Status: review
- Phase: phase-1-red
- Type: security + architecture
- Priority: critical
- Parent: LISS-0055
- Related: LISS-0044, LISS-0064

## Scope

クライアントとサーバーが共有できるVersion 1の固定ヘッダー・payload長・Command種別・session IDのバイナリcodecを実装する。socket read/write、暗号化、圧縮、互換Version 2は対象外とする。

## Frame contract

- Header: version `uint16`、command type `uint16`、session ID `int64`、payload length `uint32`。
- Integer byte order: big endian。
- Payload maximum: `MAX_COMMAND_PAYLOAD`。
- Decodeはフレーム全体の長さ、Version、Command、session ID、payload長を検証する。
- Login frameのsession IDは0、それ以外は正の内部IDを要求する。

## Acceptance criteria

- 有効Commandをencode/decodeすると同じ値になる。
- 短いヘッダー、長さ不一致、未知Command、未知Version、過大payloadを拒否する。
- 部分受信データを完全なframeとして扱わない。
- decode失敗をWorldへ渡さない。

実装とテスト資料を追加した。socketの部分read/write、暗号化、圧縮、Version 2互換は後続Issueで扱う。テスト・ビルドは実行していない。

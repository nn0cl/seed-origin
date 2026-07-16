# LISS-0029: Connectionの通信境界とライフサイクル

## Metadata

- Local issue ID: LISS-0029
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: feature + architecture
- Priority: medium
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: `main`

## Summary

現在の接続試作を、worldコアから分離された通信アダプターとして再設計する。

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0024
- Blocks: network integration

## Findings

- `Connection::open` はlisten後すぐsocketを閉じる。
- `closeSocket` が未実装。
- ポート番号が固定値。
- エラーを戻り値で返さない。
- 入力の認証、サイズ制限、切断、タイムアウト、バックプレッシャーがない。
- 通信コードからworld状態を直接変更する境界が未定義。

## Acceptance Notes

- 通信ポート、入力形式、認証、エラー、切断を仕様化する。
- worldコアは通信SDKやsocket APIへ依存しない。
- 入力を検証済みCommandへ変換してからコアへ渡す。
- 通信テストはAdapter Integration Testとして分離する。

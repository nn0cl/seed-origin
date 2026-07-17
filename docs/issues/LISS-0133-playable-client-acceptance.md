# LISS-0133: プレイアブルクライアント受入

- Status: proposed
- Priority: critical
- Depends on: LISS-0124〜0132

## 目的

クライアントを起動し、認証なしログイン、移動、チャット、NPC確認、攻撃・魔法、環境変化、切断・再接続までを一連の受入シナリオで確認する。

## 受入条件

- サーバー権威・20FPS・到着順・決定論を確認する。
- 2ユーザー以上の同時入力と同一結果再現を確認する。
- 不正入力、queue満杯、sequence gap、再接続で状態を破壊しない。
- クライアント描画とEffectQueueがWorld状態を直接変更しない。
- CodeQL、テスト、運用手順、ドキュメントが完了している。

## English

Validate the complete playable flow: anonymous login, movement, chat, NPCs, attacks, spells, ether changes, disconnect, and reconnect. Include multi-user determinism, malformed input, backpressure, sequence gaps, CodeQL, tests, operations, and documentation.

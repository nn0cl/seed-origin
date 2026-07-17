# LISS-0132: 再接続・backpressure・観測性

- Status: proposed
- Priority: high
- Depends on: LISS-0122, LISS-0128, LISS-0131

## 目的

再接続、送受信詰まり、更新欠落、EffectQueue滞留を運用上追跡できるようにする。

## 受入条件

- reconnect回数、Snapshot要求、sequence gap、queue深度、切断理由を記録する。
- backpressure時にWorld更新と表示エフェクトを区別する。
- 期限切れ・古い接続・未認証接続を安全に掃除する。
- 個人情報やチャット本文をログに残さない。
- 障害復旧手順を文書化する。

## English

Make reconnects, backpressure, missed updates, and effect backlog observable without logging personal data or chat contents. Distinguish authoritative updates from presentation effects and document recovery.

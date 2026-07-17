# LISS-0138: MP・クールダウン・二重実行防止

- Status: proposed
- Phase: phase-1-red
- Type: feature + security + determinism
- Priority: critical
- Depends on: LISS-0137, LISS-0126

## 目的

攻撃・魔法にWorldTick基準のクールダウン、MPコスト、request IDによる二重実行防止を追加する。

## 受入条件

- ability／spell ID、MP cost、cooldownをサーバー設定から解決する。
- クライアント時刻や申告MPを信用しない。
- 不足MP、cooldown中、期限切れrequestを状態変更なしで拒否する。
- 同一request IDを一度だけ確定し、再送は同じ結果を返すか明示的に重複拒否する。
- HP、MP、エーテル、cooldown更新を一つの確定処理にする。

## English

Add server-authoritative WorldTick cooldowns, MP costs, and request-id idempotency for attacks and spells. Never trust client time or resource claims; reject invalid requests without state mutation.

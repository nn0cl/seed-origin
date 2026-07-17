# LISS-0138: MP・クールダウン・二重実行防止

- Status: review
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

## 実装資料

Attack／CastSpell payloadを`requestId|targetId,power`または`requestId|targetId,element,power`形式へ拡張し、ネットワークCommand経路ではrequest IDを必須にした。WorldInputQueueは送信者内部IDとrequest IDの組を一度だけ受け付ける。攻撃はMP 0・1 tick、魔法は`ceil(basePower * 0.1)`（最低1）MP・2 tickをWorldTick基準でサーバー側が確定し、同一frame内の同一種別の連続使用、不足MP、cooldown中を状態変更なしで拒否する。適用途中の失敗時にはPlayer、エーテル、cooldown、結果Eventをframe単位でロールバックする。結果Eventには`mpSpent`と`cooldownUntil`を含める。重複requestはWorldInputQueueで明示的に拒否する。テスト・ビルドは実行していない。

## English

Add server-authoritative WorldTick cooldowns, MP costs, and request-id idempotency for attacks and spells. Never trust client time or resource claims; reject invalid requests without state mutation.

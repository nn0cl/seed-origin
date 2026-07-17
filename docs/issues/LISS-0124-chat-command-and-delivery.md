# LISS-0124: チャットCommandと配信

- Status: proposed
- Priority: medium
- Depends on: LISS-0120, LISS-0123, LISS-0059

## 目的

認証なしセッションでも利用できるサーバー権威チャットを実装する。入力検証、送信者内部ID、宛先範囲、順序、レート制限を明確化する。

## 受入条件

- 最大長、空白、制御文字、UTF-8、不正ペイロードを検証する。
- 送信者内部IDをサーバーが付与し、クライアント入力を信用しない。
- ワールド／近傍／個別など宛先を明示する。
- WorldUpdate Eventとして順序付き配信する。
- flood、なりすまし、ログ漏洩を防ぐ。

## English

Implement authoritative chat for anonymous sessions with bounded UTF-8 input, server-owned sender identity, explicit audience, ordered delivery, rate limits, and anti-spoofing/logging rules.

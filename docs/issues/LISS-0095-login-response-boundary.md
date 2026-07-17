# LISS-0095: Login応答と接続単位の送信境界

- Status: proposed
- Phase: phase-1-red
- Type: architecture + feature + security
- Priority: critical
- Parent: LISS-0041
- Depends on: LISS-0055, LISS-0072, LISS-0091, LISS-0094

## 目的

サーバー側で確定したLogin結果を、クライアントへ返すプロトコル応答として定義する。
Commandの受信・World処理・応答送信を分離し、接続ごとの送信キューから順序を保って
書き出せる準備を行う。

## 先に決める契約

- 成功・拒否・プロトコルエラーを区別する応答種別。
- クライアントへ公開する識別子。内部IDをそのまま公開するか、接続用の公開IDを別にするか。
- 応答の最大サイズ、送信キュー上限、切断時の破棄方針。
- 同一接続上の応答順序と、WorldUpdateのsequenceとの関係。
- 未認証の仮セッションが応答を受けられる範囲。

## 受入条件

- Login成功・拒否の応答を同一のversioned frame契約で表現できる。
- 応答が接続単位の送信キューへ入り、World状態を直接変更しない。
- 送信キューが上限を超えた場合に接続を安全に停止できる。
- 内部ID、申告ID、エラー文字列の漏洩方針が文書化される。
- 部分書き込み、再送、切断、重複Loginを別Issueへ分解できる。

## 非スコープ

認証、永続化、暗号化、TLS、World参加、クライアントUI、実ネットワークE2Eは扱わない。

## 分割候補

- LISS-0096: LoginResponse schema and frame codec（実装済み）
- LISS-0097: per-session bounded outbound queue（実装済み）
- LISS-0098: non-blocking partial-write boundary（実装済み）
- LISS-0099: duplicate-login and disconnect lifecycle

実装開始前に、公開識別子とエラー情報の取り扱いを受入仕様として確定する。

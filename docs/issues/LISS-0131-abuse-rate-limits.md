# LISS-0131: 不正入力・レート制限

- Status: review
- Phase: phase-3-refactor
- Priority: critical
- Depends on: LISS-0123, LISS-0124, LISS-0125, LISS-0126

## 目的

匿名利用を許容したまま、チャット flood、Command flood、座標・威力改ざん、接続枯渇を抑止する。

## 受入条件

- 接続、ログイン、Command種別、bytes、WorldInputごとに上限を設ける。
- 上限超過はWorld状態を変更せず、再試行可能な応答を返す。
- 送信元だけに依存しない内部セッション単位の制限を持つ。
- メモリ上限と切断時解放を監視できる。
- CodeQL・レビュー・運用ログで検証可能にする。

## Implementation slice

- `CommandRateLimiter`はWorldTickごとにセッション・Command種別の受付数を
  リセットする。Moveは20、Chat/Attack/CastSpellは各4、Disconnectは2を上限とする。
- `ServerCommandDispatcher`はrate limit超過をWorldInputQueue投入前に拒否し、
  active sessionごとに独立したカウンタを持つ。
- `ServerRuntime`は接続ごとの未処理Commandを128件に制限し、全体上限1024件と
  合わせて接続・メモリ枯渇を抑止する。上限計算のsize_tアンダーフローも拒否する。
- Movementは既存のpayload検証後、WorldInputQueueへ投入する経路を追加した。
- 実時間、IP/端末識別、永続監査ログ、切断タイムアウトは外部ポートと運用仕様が
  必要なため後続Issueへ残す。

## Verification

- RateLimiterのセッション分離、種別別上限、WorldTickリセット、切断後のカウンタ解放を
  テスト資料として追加した。
- `git diff --check`と警告有効C++20ビルドのみ実行。テスト、成果物、サーバーは未実行。

## English

Protect anonymous play from chat and command floods, forged movement/power, and connection exhaustion using bounded per-connection/session limits with safe rejection and observable memory release.

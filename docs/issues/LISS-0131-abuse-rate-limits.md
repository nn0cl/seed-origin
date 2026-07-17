# LISS-0131: 不正入力・レート制限

- Status: proposed
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

## English

Protect anonymous play from chat and command floods, forged movement/power, and connection exhaustion using bounded per-connection/session limits with safe rejection and observable memory release.

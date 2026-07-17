# LISS-0128: クライアント通信シェル

- Status: proposed
- Priority: high
- Depends on: LISS-0121, LISS-0122, LISS-0044

## 目的

接続、仮ログイン、Command送信、WorldUpdate受信、Snapshot再要求、切断を扱うクライアント通信シェルを作る。

## 受入条件

- 接続状態とログイン状態を明示する。
- 部分送受信、送信FIFO、再接続、タイムアウトを安全に扱う。
- Snapshot要求中はEventをゲーム状態へ渡さない。
- UIや描画を通信コードへ混ぜない。
- 通信エラーを再現可能な状態機械として記録する。

## English

Build a client transport shell for connect, anonymous login, commands, WorldUpdates, snapshot requests, and disconnect. Keep state-machine, partial I/O, FIFO, reconnect, timeout, and UI separation explicit.

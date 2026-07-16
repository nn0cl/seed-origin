# Network Protocol v1

## Purpose

クライアントとゲームサーバー間で、検証可能なCommandを運ぶ最小のバイナリ契約。

## Frame layout

| Offset | Size | Field | Encoding |
| ---: | ---: | --- | --- |
| 0 | 2 | protocol version | unsigned, big endian |
| 2 | 2 | command type | unsigned, big endian |
| 4 | 8 | session ID | signed 64-bit, big endian |
| 12 | 4 | payload length | unsigned, big endian |
| 16 | N | payload | opaque command-specific bytes |

`N`は`MAX_COMMAND_PAYLOAD`以下でなければならない。Version 1のCommand typeはLogin、Move、Chat、Attack、CastSpell、Disconnectである。

Loginだけsession ID 0を許可し、それ以外はサーバーが発行した正の内部IDを要求する。クライアントは攻撃結果、魔法結果、座標、時刻を権威値として送信しない。

TCPの部分read/write、切断、タイムアウト、再送はConnection/transport adapterで処理し、codecは完全なframe単位だけを受け取る。codecはWorld状態を変更しない。

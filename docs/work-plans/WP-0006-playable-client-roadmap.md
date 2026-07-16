# WP-0006: クライアント接続可能なプレイアブルMVPロードマップ

## Definition of the final milestone

クライアントアプリを起動し、ゲームサーバーへ接続して仮ログインする。サーバーからWorldスナップショットを受信し、3D空間でプレイヤーを表示・移動できる。チャットを送受信し、NPCを確認し、攻撃・魔法を実行して結果と環境エーテル変化を受信できる。切断・再接続・不正入力でもサーバー状態を破壊しない。

## Work packages

| Issue | Area | Priority | Depends on | Status |
| --- | --- | --- | --- | --- |
| LISS-0055 | Protocol version/schema | critical | LISS-0044 | proposed |
| LISS-0070 | Command validation slice | critical | LISS-0044 | review |
| LISS-0056 | Server executable/runtime | critical | LISS-0023, LISS-0054 | proposed |
| LISS-0057 | Login command adapter | high | LISS-0042, LISS-0055 | proposed |
| LISS-0058 | Authoritative world command pipeline | critical | LISS-0020, LISS-0022, LISS-0055 | proposed |
| LISS-0059 | Chat implementation | medium | LISS-0045, LISS-0058 | proposed |
| LISS-0060 | 3D movement implementation | high | LISS-0048, LISS-0058 | proposed |
| LISS-0061 | Attack/magic implementation | high | LISS-0046, LISS-0058 | proposed |
| LISS-0062 | NPC implementation | high | LISS-0047, LISS-0058 | proposed |
| LISS-0063 | Snapshot replication | high | LISS-0049, LISS-0058 | proposed |
| LISS-0064 | Client technology and shell | medium | LISS-0055 | proposed |
| LISS-0065 | Client login/play UI | medium | LISS-0057, LISS-0063, LISS-0064 | proposed |
| LISS-0066 | Identity persistence | high | LISS-0043, LISS-0057 | proposed |
| LISS-0067 | Abuse controls implementation | critical | LISS-0051, LISS-0058 | proposed |
| LISS-0068 | Server observability/deployment | high | LISS-0056, LISS-0067 | proposed |
| LISS-0069 | Playable client E2E acceptance | critical | LISS-0059〜0068 | proposed |

## Change-ready structure

Protocol versions, command names, snapshot fields, client capabilities, persistence format, and deployment variables must be documented before implementation. New features are added as child Issues under LISS-0041 and must not bypass the authoritative command pipeline.

## Gate order

1. LISS-0055〜0058 establish a runnable server boundary.
2. LISS-0060〜0063 make the world playable without a client.
3. LISS-0064〜0065 add a client against the versioned protocol.
4. LISS-0066〜0068 make operation and reconnect safe.
5. LISS-0069 validates the complete flow.

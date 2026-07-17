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
| LISS-0072 | Login command handler slice | high | LISS-0057, LISS-0070 | review |
| LISS-0073 | Server runtime/command queue slice | critical | LISS-0056, LISS-0058 | review |
| LISS-0074 | Protocol v1 frame codec slice | critical | LISS-0055, LISS-0064 | review |
| LISS-0075 | Stream frame accumulator slice | critical | LISS-0044, LISS-0074 | review |
| LISS-0076 | Server command dispatch boundary | critical | LISS-0058, LISS-0057 | review |
| LISS-0077 | Runtime-to-dispatch bridge | critical | LISS-0058, LISS-0076 | review |
| LISS-0078 | Movement command validation and Field application | critical | LISS-0060, LISS-0058 | review |
| LISS-0079 | 20 FPS ActionQueue frame contract | critical | LISS-0023, LISS-0022 | review |
| LISS-0080 | Snapshot/Event envelope contract | critical | LISS-0049, LISS-0055 | proposed |
| LISS-0081 | Server tick and ActionQueue implementation | critical | LISS-0079, LISS-0058 | review |
| LISS-0086 | Logical server tick/ActionQueue bridge | critical | LISS-0081, LISS-0080 | review |
| LISS-0082 | Client Snapshot application/interpolation | high | LISS-0063, LISS-0080 | proposed |
| LISS-0083 | Client EffectQueue | high | LISS-0065, LISS-0080 | proposed |
| LISS-0084 | Sync gaps/backpressure/effect backlog | high | LISS-0080, LISS-0083 | proposed |
| LISS-0085 | 20Hz server/client timing E2E | critical | LISS-0081〜0084, LISS-0069 | proposed |
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
2. LISS-0079, LISS-0080, LISS-0081 establish the 20Hz authoritative frame and wire results.
3. LISS-0060〜0063 make the world playable without a client.
4. LISS-0064〜0065, LISS-0082, LISS-0083 add a client with independent rendering/effects.
5. LISS-0066〜0068, LISS-0084 make operation, congestion, and reconnect safe.
6. LISS-0069, LISS-0085 validate the complete flow.

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
| LISS-0080 | Snapshot/Event envelope contract | critical | LISS-0049, LISS-0055 | in_progress |
| LISS-0081 | Server tick and ActionQueue implementation | critical | LISS-0079, LISS-0058 | review |
| LISS-0086 | Logical server tick/ActionQueue bridge | critical | LISS-0081, LISS-0080 | review |
| LISS-0087 | WorldUpdate envelope validation slice | critical | LISS-0080, LISS-0086 | review |
| LISS-0088 | WorldUpdate sequence tracker | critical | LISS-0080, LISS-0087 | review |
| LISS-0089 | Snapshot resync decision controller | critical | LISS-0084, LISS-0088 | review |
| LISS-0090 | Non-blocking connection accept boundary | critical | LISS-0056, LISS-0054 | review |
| LISS-0091 | ClientSession/frame boundary | critical | LISS-0056, LISS-0075 | review |
| LISS-0093 | Session command ingress | critical | LISS-0056, LISS-0091 | review |
| LISS-0094 | Runtime-to-login dispatch slice | critical | LISS-0072, LISS-0076, LISS-0077, LISS-0093 | review |
| LISS-0095 | Login response and outbound boundary | critical | LISS-0055, LISS-0072, LISS-0091, LISS-0094 | proposed |
| LISS-0096 | LoginResponse schema and frame codec | critical | LISS-0055, LISS-0074, LISS-0094 | review |
| LISS-0097 | Per-session bounded outbound frame queue | critical | LISS-0096, LISS-0091 | review |
| LISS-0098 | Non-blocking outbound write boundary | critical | LISS-0091, LISS-0097 | review |
| LISS-0099 | Session lifecycle and duplicate login | critical | LISS-0053, LISS-0072, LISS-0094, LISS-0098 | review |
| LISS-0100 | Runtime client session collection | critical | LISS-0090, LISS-0091, LISS-0098, LISS-0099 | review |
| LISS-0101 | Runtime client frame processing | critical | LISS-0094, LISS-0096, LISS-0098, LISS-0100 | review |
| LISS-0102 | Runtime session lifecycle integration | critical | LISS-0099, LISS-0100, LISS-0101 | review |
| LISS-0103 | Server frame processing order | critical | LISS-0098, LISS-0100, LISS-0101, LISS-0102 | review |
| LISS-0104 | Runtime ActionQueue frame bridge | critical | LISS-0079, LISS-0081, LISS-0086, LISS-0103 | review |
| LISS-0105 | FrameActions to WorldUpdate conversion | critical | LISS-0080, LISS-0087, LISS-0088, LISS-0104 | review |
| LISS-0106 | Deterministic world frame application | critical | LISS-0105, LISS-0020, LISS-0022 | review |
| LISS-0107 | Movement command frame input queue | high | LISS-0078, LISS-0104 | review |
| LISS-0108 | Movement intent world application | high | LISS-0106, LISS-0107 | review |
| LISS-0109 | Unified Action and Movement input queue | critical | LISS-0079, LISS-0107, LISS-0108 | review |
| LISS-0110 | Runtime and applier unified-input migration | critical | LISS-0109, LISS-0103, LISS-0106 | review |
| LISS-0111 | Unified input Event and Field ordering | critical | LISS-0110, LISS-0080, LISS-0106 | review |
| LISS-0112 | Combat and spell intent input boundary | high | LISS-0109, LISS-0110, LISS-0111 | review |
| LISS-0113 | Combat and spell world application | high | LISS-0112, LISS-0020, LISS-0022 | review |
| LISS-0114 | Environment ether, conductivity and decay | high | LISS-0113, LISS-0104 | review |
| LISS-0115 | Ether interaction and hazard severity | high | LISS-0114, LISS-0113 | review |
| LISS-0116 | Hazard WorldUpdate event | high | LISS-0115, LISS-0105, LISS-0106 | review |
| LISS-0117 | Environment snapshot aggregation | high | LISS-0114, LISS-0115, LISS-0116, LISS-0080 | review |
| LISS-0118 | Client environment snapshot state | high | LISS-0117, LISS-0080 | review |
| LISS-0119 | Client hazard effect queue | high | LISS-0116, LISS-0118 | review |
| LISS-0120 | WorldUpdate delivery boundary | high | LISS-0117, LISS-0118, LISS-0119, LISS-0056 | review |
| LISS-0121 | Client WorldUpdate receiver | high | LISS-0118, LISS-0119, LISS-0120 | review |
| LISS-0122 | Reconnect Snapshot recovery | critical | LISS-0121, LISS-0089 | review |
| LISS-0123 | Anonymous login and alias reconciliation | high | LISS-0042, LISS-0053, LISS-0066 | review |
| LISS-0124 | Chat command and delivery | medium | LISS-0120, LISS-0123, LISS-0059 | review |
| LISS-0125 | Authoritative 3D movement | high | LISS-0108, LISS-0120 | proposed |
| LISS-0126 | Authoritative combat and spells | high | LISS-0113, LISS-0115, LISS-0120 | proposed |
| LISS-0127 | Authoritative NPC lifecycle | high | LISS-0125, LISS-0126, LISS-0047 | proposed |
| LISS-0128 | Client transport shell | high | LISS-0121, LISS-0122, LISS-0044 | proposed |
| LISS-0129 | Client play shell | medium | LISS-0128, LISS-0125, LISS-0126, LISS-0127 | proposed |
| LISS-0130 | Identity persistence and reconciliation | high | LISS-0123, LISS-0053 | proposed |
| LISS-0131 | Abuse controls and rate limits | critical | LISS-0123, LISS-0124, LISS-0125, LISS-0126 | proposed |
| LISS-0132 | Reconnect, backpressure, observability | high | LISS-0122, LISS-0128, LISS-0131 | proposed |
| LISS-0133 | Playable client acceptance | critical | LISS-0124〜0132 | proposed |
| LISS-0082 | Client Snapshot application/interpolation | high | LISS-0063, LISS-0080 | proposed |
| LISS-0083 | Client EffectQueue | high | LISS-0065, LISS-0080 | proposed |
| LISS-0084 | Sync gaps/backpressure/effect backlog | high | LISS-0080, LISS-0083 | in_progress |
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

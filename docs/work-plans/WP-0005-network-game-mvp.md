# WP-0005: ネットゲームMVP計画

## Goal

認証なしの仮ログインから、チャット、移動、NPC、攻撃・魔法、同期、クライアントまでをサーバー権威・決定論的worldへ段階的に接続する。

## Safety principles

- 認証なしログインは匿名・仮セッションであり、認証済みと表示しない。
- 内部ID、座標、戦闘結果、魔法結果、NPC状態はサーバーが決定する。
- クライアント入力は通信アダプターで検証してからCommand化する。
- チャット・攻撃・魔法・移動にはサイズ、頻度、権限、対象、距離の制約を設ける。
- ID名寄せは永続化・削除・漏洩方針を決めるまで本番運用しない。

## Issue graph

| Issue | Area | Priority | Depends on | Status |
| --- | --- | --- | --- | --- |
| LISS-0041 | Network game MVP parent | high | safety gates | in_progress |
| LISS-0042 | Anonymous session/internal ID | high | LISS-0019 | review |
| LISS-0053 | Session registry implementation | high | LISS-0042 | review |
| LISS-0054 | Connection listener lifecycle | high | LISS-0029 | review |
| LISS-0070 | Network command validation | critical | LISS-0044 | review |
| LISS-0043 | Identity persistence | high | LISS-0053 | proposed |
| LISS-0044 | Network frame/command boundary | critical | LISS-0029, LISS-0042 | proposed |
| LISS-0045 | Chat | medium | LISS-0044, LISS-0051 | proposed |
| LISS-0046 | Attack/magic commands | high | LISS-0020, LISS-0022, LISS-0044 | proposed |
| LISS-0047 | NPC lifecycle | high | LISS-0020, LISS-0023 | proposed |
| LISS-0048 | 3D movement | high | LISS-0020, LISS-0023 | proposed |
| LISS-0049 | Snapshot/synchronization | high | LISS-0023, LISS-0044, LISS-0046〜0048 | proposed |
| LISS-0050 | Client application | medium | LISS-0044, LISS-0049 | proposed |
| LISS-0051 | Abuse/input controls | critical | LISS-0042, LISS-0044 | proposed |
| LISS-0052 | MVP acceptance | high | LISS-0043〜0051 | proposed |

## Execution order

1. Finish existing connection boundary and main-branch operation issues.
2. Complete LISS-0042/0053, then define persistence before public use.
3. Implement LISS-0044 and LISS-0051 before exposing gameplay commands.
4. Implement movement, NPC, combat/magic and synchronization in separate slices.
5. Build the client only against the versioned protocol and snapshots.
6. Run integration acceptance only after CodeQL, tests and operational gates are available.

## Current implementation note

LISS-0053 provides an in-memory session registry only. It is not an authentication system and is not sufficient for public deployment.

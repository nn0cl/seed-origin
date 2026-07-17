# LISS-0085: 20Hzサーバー・クライアント描画分離E2E

- Status: proposed
- Phase: phase-3-acceptance
- Type: acceptance
- Priority: critical
- Parent: LISS-0069
- Related: LISS-0081, LISS-0082, LISS-0083, LISS-0084

## Scope

複数クライアントが20Hzサーバーへ接続し、同一フレームのActionが受付順に処理され、Snapshot/Eventがクライアントへ届き、異なる描画FPSで状態とEffectが正しく表示されることを確認する。

## Acceptance criteria

- 20Hz固定フレームで複数ユーザーのAction順序が再現される。
- Snapshot反映とEffect再生が独立して進む。
- EffectQueueが詰まっても権威状態が遅延しない。
- パケット欠落・重複・遅延・再接続から復旧する。
- 移動、チャット、攻撃、魔法、NPC、エーテル変化を一連のシナリオで確認する。

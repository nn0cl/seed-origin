# LISS-0084: 同期欠落・遅延・EffectQueue詰まり対策

- Status: proposed
- Phase: phase-2-integration
- Type: reliability + performance
- Priority: high
- Parent: LISS-0049
- Related: LISS-0080, LISS-0082, LISS-0083, LISS-0067

## Scope

ネットワーク遅延、重複、欠落、サーバー更新量超過、クライアントEffectQueueの蓄積に対する復旧方針を定義・実装する。

## Acceptance criteria

- sequence gapを検知してSnapshot再取得へ遷移する。
- 古いSnapshot/Eventを無制限に保持しない。
- EffectQueueに上限と優先度を設ける。
- 画面外・低優先度Effectの省略または圧縮が可能である。
- サーバー状態が演出の遅延に引きずられない。
- 再接続後に最新状態へ収束する。

Active slice: LISS-0089（sequence gapからSnapshot再同期判断）。

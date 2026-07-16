# LISS-0024: 通信境界・スナップショット・運用受入条件

## Metadata

- Local issue ID: LISS-0024
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: architecture + feature
- Priority: medium
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `feature/LISS-0024-world-boundary`

## Summary

ネットワーク・永続化・AI推論をシミュレーションコアの外側へ配置する境界を定義し、スナップショット、イベント通知、再接続、将来AIアダプターの受入条件を整理する。

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0023
- Blocks: 統合判断

## Acceptance Notes

- コアが通信SDK、DB、AI SDKへ直接依存しない。
- 入力コマンドと出力スナップショットの契約が定義されている。
- 外部入力の時刻・連番・再送ポリシーが定義されている。
- AI推論は補助的・非権威で、コア状態を直接書き換えない。
- 通信・永続化テストは別のAdapter Integration Testとして分離する。
- 今回は外部サービスを起動せず、テスト仕様だけを作成する。

# LISS-0087: WorldUpdate Envelope検証スライス

- Status: review
- Phase: phase-1-red
- Type: protocol + safety
- Priority: critical
- Parent: LISS-0080
- Related: LISS-0055, LISS-0063, LISS-0086

## Scope

サーバーWorld結果をSnapshot/Eventとしてtransport adapterへ渡す型付きEnvelopeを定義し、version、kind、worldTick、sequence、event ID、payload上限を検証する。

## Acceptance criteria

- SnapshotとEventを別のkindとして表現できる。
- Snapshotはevent IDを持たず、Eventは正のevent IDを持つ。
- sequenceは正数であることを検証する。
- payloadに上限を設ける。
- Envelope検証はWorld状態を変更しない。
- 欠落検出、重複排除、wire encodingは後続Issueの責務として分離する。

WorldUpdate型と検証を実装した。テスト・ビルドは運用規約により実行していない。

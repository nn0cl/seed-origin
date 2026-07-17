# LISS-0116: エーテルhazardのWorldUpdate表現

- Status: review
- Phase: phase-1-red
- Type: feature + synchronization
- Priority: high
- Parent: LISS-0049
- Depends on: LISS-0115, LISS-0105, LISS-0106

## 目的

環境エーテルの副作用強度をWorldUpdate Eventとしてクライアント・観測系へ伝える。
hazardをEntityへ直接適用せず、Worldの決定結果を同期可能なデータとして公開する。

## 受入条件

- instabilityが1を超えるframeでhazard Eventを生成する。
- Event payloadにseverityとinstabilityを含める。
- hazard Eventが通常Eventと同じworldTick・sequence・eventId契約を使う。
- hazardがないframeでは余計なEventを生成しない。
- hazard Event生成はPlayer／NPC／対象物の状態を直接変更しない。
- hazardの対象物適用、クライアント演出、Snapshot集約は後続Issueで扱う。

## 実装資料

WorldFrameUpdateBuilderへhazard Event生成を追加し、WorldFrameApplierから不安定環境を
同期可能にした。テスト・ビルドは実行していない。

## 次のIssue

LISS-0117でhazard Eventの状態をSnapshotへ集約した。クライアント表示・エフェクトキューは
後続Issueで実装する。

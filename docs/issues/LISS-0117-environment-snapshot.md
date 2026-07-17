# LISS-0117: 環境エーテルSnapshot集約

- Status: review
- Phase: phase-1-red
- Type: feature + synchronization
- Priority: high
- Parent: LISS-0049
- Depends on: LISS-0114, LISS-0115, LISS-0116, LISS-0080

## 目的

環境エーテル4属性とhazard値をSnapshotへ集約し、WorldUpdate sequence契約に従う再同期
データを生成する。Event欠落時にクライアントが環境状態を再構築できる形式を用意する。

## 受入条件

- fire／water／earth／airの現在値をSnapshotへ含める。
- hazard値をSnapshotへ含める。
- SnapshotはeventId=0、正のsequence、対象worldTickを持つ。
- payloadが既存WorldUpdate検証を通過する。
- 負値hazard、非有限hazardを拒否する。
- Player／NPC一覧、演出、ソケット送信は後続Issueで扱う。

## 実装資料

`WorldSnapshotBuilder`を追加し、環境状態とhazardをSnapshot payloadへ集約した。
テスト・ビルドは実行していない。

## 次のIssue

クライアント側のSnapshot適用状態を追加する（LISS-0118）。hazard EventのEffectQueue変換はLISS-0119で扱う。

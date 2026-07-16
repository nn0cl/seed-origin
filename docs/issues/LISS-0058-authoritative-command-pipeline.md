# LISS-0058: サーバー権威Commandパイプライン

- Status: in_progress
- Phase: phase-1-red
- Priority: critical
- Parent: LISS-0041
- Depends on: LISS-0020, LISS-0022, LISS-0055

検証済みCommandを固定順序でWorldへ適用し、結果イベントと拒否理由を返す共通経路を実装する。クライアントの結果・時刻・座標を権威値として採用しない。

Active slice: LISS-0073（検証済みCommandの有界FIFO入力）, LISS-0076（Login dispatch境界）。

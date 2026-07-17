# LISS-0063: スナップショット・差分同期実装

- Status: proposed
- Phase: phase-1-red
- Priority: high
- Parent: LISS-0041
- Depends on: LISS-0049, LISS-0058

ログイン直後の初期スナップショット、連番付き差分イベント、欠落検出、再同期を実装する。

State application must remain separate from presentation effects. A late or duplicated Event may be dropped after identity checks, while a Snapshot may replace the displayed authoritative state.

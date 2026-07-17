# LISS-0090: 非ブロッキングConnection受理境界

- Status: review
- Phase: phase-1-red
- Type: network + reliability
- Priority: critical
- Parent: LISS-0056
- Related: LISS-0029, LISS-0054, LISS-0075

## Scope

リスナーsocketを非ブロッキング化し、pending clientがない状態でServerRuntimeを停止させず、受理成功・接続待ち・受理失敗を明確に区別する。

## Acceptance criteria

- listenerのbind/listen後に非ブロッキング設定を行う。
- pending clientがない場合は`NoPendingClient`を返し、待機でブロックしない。
- client受理成功時にclient socket descriptorを返す。
- listener未起動時とOSエラーを`Failed`として扱う。
- listener停止時に未受理socketを残さない。
- socket受理はWorld状態を変更しない。

非ブロッキングaccept境界を実装した。client session管理、read/write、FrameAccumulator接続は後続Issueで扱う。テスト・ビルドは運用規約により実行していない。

# LISS-0073: ServerRuntime・検証済みCommandキュースライス

- Status: review
- Phase: phase-1-red
- Type: architecture + safety
- Priority: critical
- Parent: LISS-0056
- Related: LISS-0058, LISS-0067

## Scope

Connectionを所有するサーバー実行状態と、検証済みNetworkCommandだけを受け付ける有界FIFOキューを追加する。socket accept、固定時間World更新、結果送信は後続スライスで扱う。

## Acceptance criteria

- 起動前・起動中・停止後の状態遷移を明示する。
- listenerの起動失敗をサーバー起動失敗として返す。
- 無効Commandをキューへ入れない。
- キュー上限を超える入力を拒否する。
- drain順序が投入順で決定的になる。
- stopでlistenerと保留Commandを安全に解放する。

実装とテスト資料を追加した。socket accept、World更新、snapshot送信、実際のサーバー常駐は後続スライスで扱う。テスト・ビルドは実行していない。

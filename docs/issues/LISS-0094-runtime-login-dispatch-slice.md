# LISS-0094: runtimeからLogin処理へのディスパッチ接続

- Status: review
- Phase: phase-1-red
- Type: feature + integration
- Priority: critical
- Parent: LISS-0056
- Depends on: LISS-0072, LISS-0076, LISS-0077, LISS-0093

## 目的

サーバー実行時に取り込まれたLogin Commandを、到着順を保ったまま
`ServerCommandDispatcher`へ渡し、`SessionRegistry`が発行した仮セッション結果を
サーバー側で受け取れる境界を明示する。まだクライアントへのLogin応答、World参加、
認証、永続化は扱わない。

## 受入条件

- Runtimeの保留CommandがFIFO順にディスパッチされる。
- Login Commandが`LoginCommandHandler`を経由してSessionRegistryへ到達する。
- 成功結果に内部IDが含まれ、認証済みとは表示されない。
- ディスパッチ後に処理済みCommandがRuntimeキューに残らない。
- socketへの応答、World状態変更、認証・永続化をこのスライスへ混入させない。

## 実装資料

Runtimeから既存のDispatcherへLoginを接続する経路と、仮セッション結果を確認する
テスト資料を追加した。テスト・ビルドは実行していない。

## 次のIssue

Login結果をネットワーク応答フレームへ変換し、接続単位の応答キューへ渡す境界を
別Issueとして定義する。応答に内部IDを含める場合の漏洩範囲もそこで確定する。

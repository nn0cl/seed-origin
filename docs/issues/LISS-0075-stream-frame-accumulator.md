# LISS-0075: TCP部分read用FrameAccumulatorスライス

- Status: review
- Phase: phase-1-red
- Type: security + architecture
- Priority: critical
- Parent: LISS-0044
- Related: LISS-0056, LISS-0074

## Scope

TCPの部分read・複数frame同時readを、Protocol v1 codecへ渡す前に蓄積・分割する。接続ごとのdecoder状態を分離し、最大frame長を超える入力を拒否する。

## Acceptance criteria

- header未満の入力ではCommandを生成しない。
- 分割されたheader/payloadを結合して一つのCommandにする。
- 一回のreadに複数frameが含まれる場合、順序を保持して全て返す。
- 宣言payload長が上限を超える場合、蓄積を停止して失敗する。
- 完成frameはNetworkFrameCodecの検証を通過したCommandだけを返す。
- 接続ごとに蓄積状態を持ち、他接続と混ざらない。

実装とテスト資料を追加した。socket read/write・切断・timeoutは後続スライスで扱う。テスト・ビルドは実行していない。

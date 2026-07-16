# LISS-0078: 三次元移動Commandの検証・適用

- Status: review
- Phase: phase-1-red
- Type: safety + feature
- Priority: critical
- Parent: LISS-0060
- Related: LISS-0020, LISS-0058, LISS-0077

## Scope

ネットワークのMove Commandを`dx,dy,dz`として厳格に解析し、有限値・移動量上限・Field内の既存Playerを確認してからPosition queueへ投入する。PositionのPlayer IDを64bitへ統一する。

## Acceptance criteria

- Move以外のCommandを拒否する。
- 不正形式、余分な値、NaN/無限値、上限超過を拒否する。
- Fieldに存在しないSession IDの移動を拒否する。
- 受理した移動だけが次の`processFrame`でPlayerへ適用される。
- PositionのPlayer IDがSessionRegistryと同じ64bit幅で保持される。
- テスト資料を追加する。

実装とテスト資料を追加した。テスト・ビルドは運用規約により実行していない。

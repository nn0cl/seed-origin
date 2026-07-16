# LISS-0027: Status・Position・Buffの状態ライフサイクル

## Metadata

- Local issue ID: LISS-0027
- GitHub issue: 未作成
- Status: proposed
- Phase: phase-0-design
- Type: bugfix + feature
- Priority: high
- Initial planning size: M
- Current planning size: M
- Owner/agent: Codex
- Related branch: `main`

## Summary

HP/MP、座標、バフの状態変更と寿命処理を、worldの効果モデルへ接続できる契約に整理する。

## Dependencies

- Parent: LISS-0017
- Depends on: LISS-0019
- Blocks: LISS-0022

## Findings

- HP/MP加算の整数オーバーフロー対策がない。
- 最大値、負値、回復・ダメージの仕様が未定義。
- `Position` のコピーコンストラクタがポインタ形式で、`mapId`のコピー・公開契約が不明。
- `Player::setPosition` が座標だけを更新し、識別子の整合性を検証しない。
- `Buff` の寿命・開始時刻にgetterや期限処理がない。
- `processBuffs` が同じバフを無期限に繰り返し適用する。
- 内部Statusを直接返すAPIが状態変更を無制限に許す。

## Acceptance Notes

- 数値範囲、飽和、オーバーフロー、座標境界を仕様化する。
- バフの開始、周期適用、減衰、期限切れ削除を定義する。
- 内部状態の読み取りと変更のAPI境界を分ける。
- worldのエーテル効果とPlayerの状態効果を混同しない。

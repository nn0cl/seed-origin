# LISS-0125: 権威サーバー3D移動

- Status: review
- Priority: high
- Depends on: LISS-0108, LISS-0120

## 目的

プレイヤーのx/y/z移動を20FPSのWorldInputとして処理し、速度・境界・衝突・順序をサーバーで決定する。

## 受入条件

- クライアントは移動意図だけを送信し、座標結果を指定できない。
- 速度、dt、座標範囲、NaN/Inf、衝突を検証する。
- 同一フレームの入力はsequence順に適用する。
- 結果をSnapshot/Eventへ反映する。
- 移動中の切断・再接続で状態を破壊しない。

## 実装資料

1フレームの移動距離を100以下に制限し、各成分の有限性、累積後の座標範囲（±1,000,000）をWorld適用前に検証する。Command経路とWorldInputQueue経路の両方で同じ距離制限を使用する。テスト・ビルドは実行していない。

## English

Process x/y/z movement as authoritative 20 FPS intents. Clients submit intentions, while the server validates speed, time step, bounds, finite values, collisions, ordering, and replicated results.

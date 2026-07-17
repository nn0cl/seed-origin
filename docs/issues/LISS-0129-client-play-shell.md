# LISS-0129: クライアントプレイシェル

- Status: proposed
- Priority: medium
- Depends on: LISS-0128, LISS-0125, LISS-0126, LISS-0127

## 目的

ログイン後に3D空間、プレイヤー・NPC、チャット、攻撃・魔法、環境エーテルを確認できる最小クライアントを作る。

## 受入条件

- 描画状態はSnapshotから再構築できる。
- 入力はCommandへ変換し、Worldを直接変更しない。
- EffectQueueは表示フレーム側で消化する。
- 20FPS更新とエフェクト数FPSを混同しない。
- 切断中に見せる状態を明示する。

## English

Build the smallest playable client showing the world, players, NPCs, chat, attacks, spells, and ether. Rebuild render state from snapshots, send commands through transport, and consume effects independently from server tick rate.

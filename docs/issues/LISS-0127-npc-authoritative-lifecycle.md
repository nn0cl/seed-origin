# LISS-0127: NPC権威ライフサイクル

- Status: proposed
- Priority: high
- Depends on: LISS-0125, LISS-0126, LISS-0047

## 目的

NPCの配置、識別、状態、移動、攻撃対象、死亡・再配置をサーバー権威で管理する。

## 受入条件

- NPC内部IDと種別を一意に管理する。
- 初期配置・再配置・範囲・衝突を決定論的にする。
- NPCを攻撃・魔法対象として扱える。
- Snapshotに必要な公開状態だけを含める。
- AI更新がWorld frame順序を飛び越えない。

## English

Manage NPC placement, identity, state, movement, targeting, death, and respawn authoritatively and deterministically. Expose only permitted state in snapshots and keep AI inside the World frame.

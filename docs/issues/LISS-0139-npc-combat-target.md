# LISS-0139: NPCをCombatTargetへ統合

- Status: proposed
- Phase: phase-1-red
- Type: feature + world model
- Priority: high
- Depends on: LISS-0127, LISS-0137

## 目的

Playerだけを対象とする現行Combat境界を、NPC・将来の対象物にも拡張できる共通対象契約へ整理する。

## 受入条件

- Player／NPCが一意の内部ID、位置、HP、生存状態を持つ。
- 攻撃・魔法の対象解決、距離、耐性、ダメージ適用を共通化する。
- NPCの公開状態だけをSnapshotへ含める。
- NPC配置・死亡・再配置をWorld frame順に解決する。
- NPC AIの判断とCombat結果適用を別責務にする。

## English

Extend the current player-only combat boundary to a shared target contract for players, NPCs, and future objects. Keep identity, position, health, life state, range, resistance, snapshots, placement, and AI responsibilities explicit.

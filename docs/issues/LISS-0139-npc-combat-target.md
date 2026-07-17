# LISS-0139: NPCをCombatTargetへ統合

- Status: in_progress
- Phase: phase-2-green
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

## 実装スライス

### 0139-A: 共通CombatTargetとNPC対象適用

- `Player`と`NPC`を同じtarget ID解決境界から参照できる。
- NPCは内部ID、種別、位置、HP、生存状態を値として所有する。
- 既存の攻撃・魔法Intentは、Playerまたは生存NPCへ距離検証後に適用できる。
- 死亡NPCへの攻撃・魔法は状態を変更せず拒否する。
- 既存Player向けのwire payload、結果Eventの値、WorldTick順序は変更しない。

### 0139-B: NPC公開Snapshot

- 生存・公開NPCのID、種別、位置、HPだけをSnapshotへ含める。
- 管理用・AI内部状態はSnapshotへ出さない。
- Snapshot schema変更は専用のprotocol仕様更新と互換性確認を先に行う。

### 0139-C: 配置・死亡・再配置

- 配置と再配置をWorldTick／input sequence順に適用する。
- 座標上限、ID重複、再配置待ちをサーバー側で検証する。
- 再配置を外部タイマーや通信スレッドから直接実行しない。

### 0139-D: NPC AI

- AIは意思決定だけを返し、状態変更はWorld frameの入力として適用する。
- AIの乱数・時刻・外部I/Oは決定論的な入力ポートへ隔離する。
- AI判断とCombat結果確定を同一責務へ混在させない。

## 今回の実装範囲

本着手では0139-A〜Cを実装する。0139-Dは受入資料と依存境界を先に
固定し、NPC AIの設計判断を配置・Combat実装へ混在させない。

## 実装記録

- `Npc`を値所有型として追加し、ID、種別、位置、Status、生存状態を保持する。
- `Field`にPlayer／NPCのID衝突を拒否する配置境界と、共通target位置・生存・
  ダメージ解決を追加した。
- Combat／Spellは生存NPCを対象にでき、死亡NPCは状態変更なしで拒否する。
- frameロールバックの対象へNPC一覧を追加した。
- 配置済みlive NPCの移動を位置キューへ接続し、座標上限・移動量・生存状態を
  検証してWorld frameで適用する境界を追加した。
- 死亡NPCの復活予約をNPC IDごとに保持し、指定した`worldTick`以降の
  `processFrame`でID順に復活させる境界を追加した。実時間タイマー、通信スレッド、
  クライアント入力から直接状態を変更しない。
- `tests/NpcCombatTargetTest.cpp`へlive／dead NPCの受入シナリオを追加した。
- `NpcSnapshot` DTO、公開live NPCだけを出力するSnapshot builder、クライアント側の
  NPC公開状態適用を追加した。NPC管理情報・AI情報・dead NPCは出力しない。
- C++20 warning-enabled buildは成功。テスト・成果物・サーバは実行していない。

## 設計ノート

- Included: `Field`、Combat／Spell適用、Player値型、World input／result Event。
- Omitted: Snapshot wire schema、永続化、NPC AI、外部乱数・時刻provider。
- Assumption: NPCの初期状態はサーバーが明示的に配置し、未配置NPCは攻撃対象に解決しない。
  死亡NPCの復活はサーバーが正のHP・座標・復活Tickを予約し、Tick到達後にのみ適用する。
- Verification: テストは追加するが実行せず、C++20ビルドと静的検査のみ行う。

## English

Extend the current player-only combat boundary to a shared target contract for players, NPCs, and future objects. Keep identity, position, health, life state, range, resistance, snapshots, placement, and AI responsibilities explicit.

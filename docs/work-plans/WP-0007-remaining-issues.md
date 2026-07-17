# WP-0007: 残存Issueのcanonical実行計画

## 目的

過去に作成した親Issueと、実装時に分割したLISS-0070以降のスライスを混同せず、クライアントでログインして遊べるまでの残作業を一つの依存グラフとして扱う。

## 現在地

2026-07-17時点で、通信境界、匿名セッション、20FPS WorldInput、WorldUpdate同期、環境エーテルv1、チャット入力、権威3D移動、攻撃・魔法Command接続、攻撃・魔法結果Event、MP・クールダウン、frameロールバックまで実装済み。過去のスライスには`review`が残るが、これは実装完了・Adjudicator確認待ちを表す。実装順と依存関係は本WorkPlanのcanonical表だけを正とする。

## Canonical実行順

| 順序 | Canonical Issue | 内容 | 状態 |
| --- | --- | --- | --- |
| 0 | LISS-0019 | world安全性ゲートと所有権契約 | in_progress |
| 1 | LISS-0137 | 攻撃・魔法の結果Eventと原子性 | review（実装済み） |
| 2 | LISS-0138 | MP・クールダウン・二重実行防止 | review（実装済み） |
| 3 | LISS-0139-A | 共通CombatTargetとNPC対象適用 | in_progress |
| 4 | LISS-0139-B | NPC公開Snapshot | in_progress |
| 5 | LISS-0139-C | NPC配置・死亡・再配置 | proposed |
| 6 | LISS-0139-D | NPC AIの決定論的入力 | proposed |
| 7 | LISS-0131 | 不正入力・レート制限 | proposed |
| 8 | LISS-0130 | ID名寄せの永続化と削除方針 | proposed |
| 9 | LISS-0128 | クライアント通信シェル | proposed |
| 10 | LISS-0129 | クライアントプレイシェル | proposed |
| 11 | LISS-0132 | 再接続・backpressure・観測性 | proposed |
| 12 | LISS-0133 | プレイアブルクライアント受入 | proposed |

## 追加Issue

### LISS-0138: MP・クールダウン・二重実行防止

- LISS-0126／0137の後続。
- ability／spell ID、MP cost、cooldown WorldTick、request IDを定義する。
- 失敗時のHP・MP・エーテル・cooldown不変を保証する。
- 同一request IDまたは入力IDを二重処理しない。
- 攻撃をMP 0・1 tick、魔法を`ceil(power * 0.1)`（最低1）MP・2 tickとし、結果Eventへ消費量と解禁tickを出力する。適用途中の失敗時はframe単位でロールバックし、重複requestは明示的に拒否する。

### LISS-0139: NPCをCombatTargetへ統合

- LISS-0127／0137の後続。
- PlayerとNPCを共通の対象契約へ統合する。
- NPC配置、公開状態、HP、耐性、死亡状態をWorld frameへ接続する。
- NPC AI、再配置、攻撃判断は別スライスとして分離する。
- 実装順は0139-A（対象適用）、0139-B（公開Snapshot）、0139-C（配置・死亡・再配置）、0139-D（AI）とする。

### 棚卸し結果

- LISS-0041〜0069は親Issue／初期ロードマップとして保持し、実装完了判定は
  0070以降と0137以降のcanonical sliceで行う。
- LISS-0070〜0126の`review`は、過去の実装資料とCodeQL確認の証跡を保持する
  historical sliceであり、新しい実装を重複して開始しない。
- LISS-0134は4属性からFFXI型8属性へ移行する採用判断であり、0139-Aの対象
  解決とは独立させる。属性wire schemaを変更するまで実装Issueへ昇格しない。
- LISS-0131、0130、0128、0129、0132、0133は、NPC対象境界を固定した後の
  canonical残作業として保持する。

## 重複Issueの扱い

| 旧親Issue | Canonical実装・計画 | 扱い |
| --- | --- | --- |
| LISS-0042／0053／0066 | LISS-0123／0130 | 匿名ログインと永続名寄せを分離 |
| LISS-0045／0059 | LISS-0124 | チャット入力・配信へ集約 |
| LISS-0046／0061／0112／0113 | LISS-0126／0137／0138／0139 | Command、結果、リソース、NPCへ分割 |
| LISS-0047／0062 | LISS-0127／0139 | NPCライフサイクルとCombatTargetを分離 |
| LISS-0048／0060／0078 | LISS-0125 | 権威3D移動へ集約 |
| LISS-0049／0063／0082 | LISS-0117／0118／0120〜0122 | Snapshot生成、適用、配信、再接続へ分割 |
| LISS-0050／0064／0065 | LISS-0128／0129 | 通信シェルとプレイUIへ分離 |
| LISS-0083／0084 | LISS-0119／0121／0132 | EffectQueue、受信、運用観測へ分割 |

旧親Issueは削除せず、仕様の出発点・依存関係・受入シナリオを保持する。実装着手と完了判定はCanonical Issueを正とする。

## 運用ゲート

各Canonical Issueは、受入条件、テスト資料、CodeQL確認、mainコミット、mainプッシュの順で完了とする。CodeQLの未解決指摘がある場合は、次のIssueを開始せず、最優先の修正Issueへ切り替える。

---

## English

This plan defines the canonical execution graph for the remaining work. Historical parent Issues remain as design context, while implementation and completion are tracked by the newer sliced Issues. The order is: safety gate, element migration decision, combat result atomicity, resources/idempotency, NPC targets, abuse controls, identity persistence, client transport, client play shell, observability, and playable acceptance.

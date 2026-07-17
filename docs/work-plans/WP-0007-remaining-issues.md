# WP-0007: 残存Issueのcanonical実行計画

## 目的

過去に作成した親Issueと、実装時に分割したLISS-0070以降のスライスを混同せず、クライアントでログインして遊べるまでの残作業を一つの依存グラフとして扱う。

## 現在地

2026-07-17時点で、通信境界、匿名セッション、20FPS WorldInput、WorldUpdate同期、環境エーテルv1、チャット入力、権威3D移動、攻撃・魔法Command接続、攻撃・魔法結果Event基礎まで実装済み。未完了のIssueはStatusが`proposed`または`in_progress`であり、`review`は実装資料・受入条件の確認待ちである。

## Canonical実行順

| 順序 | Canonical Issue | 内容 | 状態 |
| --- | --- | --- | --- |
| 0 | LISS-0019 | world安全性ゲートと所有権契約 | in_progress |
| 1 | LISS-0134 | FFXI型8属性移行の採用判断 | proposed |
| 2 | LISS-0137 | 攻撃・魔法の結果Eventと原子性 | review |
| 3 | LISS-0138 | MP・クールダウン・二重実行防止 | review |
| 4 | LISS-0139 | NPCをCombatTargetへ統合 | proposed |
| 5 | LISS-0131 | 不正入力・レート制限 | proposed |
| 6 | LISS-0130 | ID名寄せの永続化と削除方針 | proposed |
| 7 | LISS-0128 | クライアント通信シェル | proposed |
| 8 | LISS-0129 | クライアントプレイシェル | proposed |
| 9 | LISS-0132 | 再接続・backpressure・観測性 | proposed |
| 10 | LISS-0133 | プレイアブルクライアント受入 | proposed |

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

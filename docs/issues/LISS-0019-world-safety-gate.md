# LISS-0019: world 移行安全性ゲートとC++所有権契約

## Metadata

- Local issue ID: LISS-0019
- GitHub issue: 未作成
- Status: review
- Phase: phase-3-refactor
- Type: safety + refactor
- Priority: critical
- Initial planning size: L
- Current planning size: L
- Owner/agent: Codex
- Related branch: `main`

## Summary

world移行前に、既存C++の所有権、コピー・代入、ポインタ公開、バッファ境界、コンテナ格納契約を確定する。安全性が担保されるまで、worldの実装・コンパイル・テスト実行を行わない。

## Acceptance Notes

- `Player` のコピーコンストラクタとコピー代入の所有権契約が定義されている。
- `Field::putPlayer` の同一ID更新が浅いコピーや二重解放を起こさない設計になっている。
- `Buff`、`Action`、`Position`、`Status` の値・参照・所有ポインタの境界が文書化されている。
- `ActionQueue` の未使用生ポインタをどう扱うか決定されている。
- `SeedBinary` のサイズ・インデックス境界が仕様化されている。
- `Field::putPlayer` の暗黙コピー代入、`Action` の値コピー、コンテナ格納時の破棄順を含む所有権表がある。
- `SeedBinary::setBinary` と `FileManager::save` の境界・未初期化読み取りリスクが分類されている。
- このIssue完了まで子IssueのPhase 1/2実装を開始しない。

## Active Slice

- LISS-0034: Player所有権・コピー代入安全性（review）

## Dependencies

- Parent: LISS-0018
- Depends on: LISS-0017
- Blocks: LISS-0020, LISS-0021, LISS-0022, LISS-0023, LISS-0024

## Adjudicator Decision Points

- `Player` を値メンバーへ移行するか、`std::unique_ptr` とRule of Fiveで管理するか。
- 内部状態getterをconst参照・値・非所有ポインタのどれにするか。
- 旧API互換性をどこまで維持するか。
- Sanitizer等の実行を安全性ゲートレビュー後に許可するか。

## Phase plan

1. Design intake: 所有権表、コピー操作表、危険API一覧を作る。
2. Phase 1 Red: コピー代入、同一ID更新、破棄、境界入力を示すテストを設計・実装する。ただし本計画中は実行しない。
3. Phase 2 Green: Adjudicator承認後に最小修正を実装し、許可されたタイミングで検証する。
4. Phase 3 Refactor: 生ポインタAPIを整理し、worldが安全な値・参照契約だけを利用する状態にする。

## Bug policy

メモリ破壊、二重解放、解放済み参照、バッファオーバーフロー、データ競合を発見した場合はFast Pathを使わず、個別Issueまたは本Issueの計画記録へ追加してから修正する。

## AI Planning Records

### AIP-0019-001

- Status: proposed
- Created by: Codex desktop / local workspace
- Model as displayed: GPT-5
- Reasoning setting as displayed: N/A (not exposed)
- Created at: 2026-07-17
- Planning size: L
- Intended execution route: Architecture/Feature Path design intake, safety review, then gated TDD
- Intended scope: existing C++ ownership and memory contracts only; no world behavior implementation
- Estimated token range: 8k-16k
- Estimated token midpoint: 12k
- Token metric: planning estimate
- Estimation basis: raw owning pointers, implicit assignment, container copies, legacy binary buffers
- Assumptions: no world implementation or tool execution before gate approval
- Confidence: medium

## Ownership contract

| Type | Stored state | Ownership rule | External mutation |
| --- | --- | --- | --- |
| `Player` | `Status`, `list<Buff>`, `Position`, name array | value ownership; copy/assignment duplicate state | through explicit Player mutators only |
| `Buff` | name, lifetime, start time, `Status` | value ownership | no pointer is exposed |
| `Position` | coordinates and IDs | value type; copy constructor takes `const Position&` | no raw pointer copy API |
| `Status` | HP and MP | value type with bounded arithmetic | through validated mutators |
| `Action` | copied Player values and Status | constructor reads non-owning `const Player*`; action stores copies | getters return `const` references, never deletable pointers |
| `ActionQueue` | `deque<QueuedAction>` and mutex | queue owns value entries | callers receive copied frame values |
| `SeedBinary` | `map<int, Binary>` | map owns initialized fixed-size blocks | input/output buffers are caller-owned and size-checked |

The nullable Player pointers in `Action` are input references only and are never
stored. `Player` and `Position` no longer provide nullable pointer-copy APIs.
`ActionQueue` rejects sequence-number exhaustion before incrementing, preventing
wraparound that could duplicate frame ordering.

## Safety slice record

- Removed the nullable `Player(Player*)` copy constructor.
- Replaced `Position(const Position*)` with `Position(const Position&)`.
- Replaced mutable `Action` Player/Status getters with const-reference getters.
- Added the `ActionQueue` sequence exhaustion guard and compile-only regression test.
- C++20 warning-enabled build and `git diff --check` are the only verification;
  tests, executables, and server runtime remain unexecuted.

# LISS-0142: C++20 idiomとメモリ安全性リファクタ

## Metadata

- Local issue ID: LISS-0142
- GitHub issue: 未作成
- Status: done
- Phase: phase-0-design
- Type: refactor + safety
- Priority: high
- Initial planning size: L
- Current planning size: L
- Reclassification reason: N/A
- Owner/agent: Codex
- Related branch: `main`

## Summary

C++20標準への移行後、公開挙動を変えずに、危険な所有権・初期化・定数定義・
ループ記法を保守的にmodernizeする。目的は「最新風」ではなく、人間が安全に
レビューできる低認知負荷のコードへ寄せることである。

## Acceptance Notes

- C-style integer/size headersを、触る範囲で `<cstdint>`/`<cstddef>` へ移行する。
- 共有ヘッダ定数は `inline constexpr` へ寄せる。
- ownership/value型のconstructorはmember initializer listを使う。
- 危険なraw owning pointer追加は禁止。既存の値所有を維持する。
- 反復処理は読みやすい箇所でrange-based `for` へ更新する。
- `[[nodiscard]]` を、戻り値無視が危険なvalidate/encode/decode/queue系へ
  適用する。
- behaviorやwire protocol値は変更しない。
- テスト・成果物は実行しない。

## Dependencies

- Parent: WP-0008
- Depends on: LISS-0141
- Blocks: future typed-ID/API cleanup issues
- Related: ADR-0015, `include/seed/`, `src/`

## Adjudicator Decision Points

- `std::optional`/`std::span`/strong typed IDなど公開APIを変える改善は後続Issue。
- Xcode projectの明示C++20設定同期は別Issueで判断可能。

## Context

- Included: ADR-0015, compile-restored source, direct modernization targets.
- Omitted: behavior redesign, modules/coroutines, client/server runtime
  execution, persistence/provider concerns.
- Assumptions: no runtime assertions are evaluated in this issue; compile/static
  checks are the only verification evidence.

## AI Planning Records

### AIP-0142-001

- Status: accepted
- Created by:
  - Agent/environment: Codex / local workspace
  - Model as displayed: fugu
  - Reasoning setting as displayed: N/A
  - N/A reason: runtime does not expose a separate user-facing reasoning label
- Created at: 2026-07-17
- Planning size: L
- Intended execution route: Feature Path Phase 3 refactor under accepted ADR
- Intended scope: conservative C++20 idiom modernization and safety cleanup
- Estimated token range: 8k-16k
- Estimated token midpoint: 12k
- Token metric: planning estimate
- Estimation basis: many headers/sources, public API caution, compile-only
  verification
- Assumptions: no test/artifact execution and no protocol behavior changes
- Confidence: medium
- Revises: N/A
- Revision reason: N/A
- Superseded by: N/A

## References

- ADR-0015: C++20 language standard adoption
- WP-0008: C++20 migration and modernization

## Work Notes

- Pending.

## Verification

- Pending: CMake build, syntax-only compile, `git diff --check`, static source
  searches.
- Not allowed: `ctest`, `seed_tests`, `seed_cli`, server runtime.

## Completion Evidence

- Shared header constants use `inline constexpr`; touched headers use
  `<cstdint>`/`<cstddef>`.
- `Player` name storage is value-initialized and copied; `Position` no longer
  accepts a mutable pointer for copying; obsolete raw-allocation code was
  removed; `ServerRuntime` uses `std::make_unique`.
- File block indexing is guarded against integer exhaustion, and aggregate
  input initialization no longer relies on omitted request-ID fields.
- Warning-enabled C++20 configure/build completed successfully. Remaining
  warnings are unused legacy fields (`Player::name`, `Position::mapId`), not
  memory or initialization diagnostics. No test or executable was run.

## Adjudicator Approval

- Approved by the Adjudicator on 2026-07-17.

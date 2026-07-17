# LISS-0141: CMakeと文書のC++20標準移行

## Metadata

- Local issue ID: LISS-0141
- GitHub issue: 未作成
- Status: done
- Phase: phase-0-design
- Type: architecture + build configuration
- Priority: high
- Initial planning size: M
- Current planning size: M
- Reclassification reason: N/A
- Owner/agent: Codex
- Related branch: `main`

## Summary

ADR-0015に基づき、CMakeのC++標準をC++20へ更新し、アーキテクチャ文書と
README相当の技術選定記述をC++11からC++20へ同期する。

## Acceptance Notes

- `CMakeLists.txt` が `CMAKE_CXX_STANDARD 20` を設定する。
- `docs/architecture/README.md` のSelected TechnologyがC++20を示す。
- `docs/architecture/seed-implementation.md` がC++20移行方針と現在の
  保守的modernization範囲を説明する。
- C++標準値以外のビルド挙動やプロトコル値を変更しない。
- テスト・成果物は実行しない。

## Dependencies

- Parent: WP-0008
- Depends on: LISS-0140, ADR-0015
- Blocks: LISS-0142
- Related: `CMakeLists.txt`, `docs/architecture/README.md`,
  `docs/architecture/seed-implementation.md`

## Adjudicator Decision Points

- Xcode projectのC++ dialect設定を同時に変更するかは未決。今回の必須ゲートは
  CMake標準移行であり、Xcode同期は後続Issue化可能。

## Context

- Included: ADR-0015, WP-0008, CMake, architecture docs.
- Omitted: external libraries, package managers, UI/client stack, runtime
  execution.
- Assumptions: CMake is the authoritative deterministic build gate for this
  migration slice.

## AI Planning Records

### AIP-0141-001

- Status: accepted
- Created by:
  - Agent/environment: Codex / local workspace
  - Model as displayed: fugu
  - Reasoning setting as displayed: N/A
  - N/A reason: runtime does not expose a separate user-facing reasoning label
- Created at: 2026-07-17
- Planning size: M
- Intended execution route: Architecture Path accepted decision + build config
  implementation
- Intended scope: CMake standard bump and documentation synchronization
- Estimated token range: 3k-6k
- Estimated token midpoint: 4.5k
- Token metric: planning estimate
- Estimation basis: small config diff plus architecture doc sync
- Assumptions: no test/artifact execution
- Confidence: high
- Revises: N/A
- Revision reason: N/A
- Superseded by: N/A

## References

- ADR-0015: C++20 language standard adoption
- WP-0008: C++20 migration and modernization

## Work Notes

- Pending.

## Verification

- Pending: CMake configure/build only.
- Not allowed: `ctest`, `seed_tests`, `seed_cli`, server runtime.

## Completion Evidence

- `CMAKE_CXX_STANDARD 20`, `CMAKE_CXX_STANDARD_REQUIRED ON`, and extensions
  disabled are active in `CMakeLists.txt`.
- Architecture documentation and the accepted ADR now state C++20.
- CMake C++20 configure/build completed successfully without changing wire
  protocol values.

## Adjudicator Approval

- Approved by the Adjudicator on 2026-07-17.

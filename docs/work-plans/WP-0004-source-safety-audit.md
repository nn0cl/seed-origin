# WP-0004: 現行C++ソース監査と実装候補Issue群

## Goal

現行C++ソースから、安全性、未実装動作、I/O、テスト、ビルド、運用の実装候補をIssue化し、LISS-0019を最優先に安全な順序で処理できるようにする。

## Execution Gate

初期監査では、移行安全性が担保されるまで実装、コンパイル、テスト実行を行わない。ユーザー承認後は安全性Issueの実装を進めるが、コンパイル・テスト実行は引き続き行わず、テストコードを検証資料として整備する。

## Issue Graph

| Issue | Area | Priority | Depends on | Status |
| --- | --- | --- | --- | --- |
| LISS-0019 | Player/Action/Buffer ownership and memory safety | critical | LISS-0017 | in_progress |
| LISS-0034 | Player ownership and copy-assignment slice | critical | LISS-0019 approval | review |
| LISS-0025 | SeedBinary/FileManager buffer and I/O safety | critical | LISS-0019 | in_progress |
| LISS-0035 | SeedBinary/FileManager boundary safety slice | critical | LISS-0025 | review |
| LISS-0026 | Field state and queue contract | high | LISS-0019 | proposed |
| LISS-0027 | Status/Position/Buff lifecycle | high | LISS-0019 | proposed |
| LISS-0028 | Action domain result contract | high | LISS-0019, LISS-0020 | proposed |
| LISS-0030 | TDD/build/test foundation | high | LISS-0019 | proposed |
| LISS-0031 | CodeQL review gate（既知不都合修正後） | critical | LISS-0019, LISS-0025〜LISS-0028, LISS-0030, LISS-0032, LISS-0033 | blocked |
| LISS-0032 | main branch migration | high | repository administration | proposed |
| LISS-0029 | Connection transport boundary | medium | LISS-0024 | proposed |
| LISS-0033 | legacy cleanup | low | related behavior decisions | proposed |

## Review order

1. LISS-0019: stop all world work and resolve ownership contract.
2. LISS-0025: remove binary and file I/O memory corruption paths.
3. LISS-0026 and LISS-0027: establish Field and state lifecycle contracts.
4. LISS-0030: review TDD/build structure without running tests in this phase.
5. LISS-0032: make main operational.
6. Process LISS-0033 last because cleanup can remove historical compatibility.
7. Only after the known defect Issues are complete, configure LISS-0031 CodeQL.
8. Continue with world child Issues only after the safety gate and CodeQL gate are approved.

## Audit categories

- **Critical safety**: shallow copy, double free, use-after-free, buffer overflow, uninitialized read, data race.
- **Correctness**: no-op methods, wrong ID type, dropped queue entries, invalid result reporting, status lifecycle.
- **Architecture**: Field/World responsibility, connection boundary, ownership API, test target.
- **Operations**: main branch, commit/push completion, CodeQL review gate.
- **Cleanup**: empty classes, obsolete tests, legacy API, comments and stale project references.

## Bug handling

All findings are recorded before correction. Critical safety or determinism findings always receive an Issue and block dependent implementation. A Fast Path is allowed only under the conditions in `docs/work-plans/WP-0003-world-integration.md` and `docs/collaboration/local-issue-planning.md`.

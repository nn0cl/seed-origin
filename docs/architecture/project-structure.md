# Project Structure

This document defines the current C++ repository shape and reserved locations
for the future server/client split. Create new directories only when a planned
Issue requires them.

## Target Layout

```text
.
├── include/seed/                 # public headers and domain contracts
├── src/                          # core, adapters, CLI and server code
├── tests/                        # CTest-compatible test sources
├── client/                       # reserved for LISS-0064 client shell
├── docs/architecture/            # architecture and ADRs
├── docs/collaboration/           # LLM operating contracts and traces
├── docs/issues/                  # local LISS issue records
├── docs/work-plans/              # dependency-aware execution plans
├── CMakeLists.txt                # portable build/test foundation
└── seed.xcodeproj/               # existing Xcode product
```

## Ownership

`client/` will own UI rendering and presentation state after LISS-0064.

`include/seed/` and the domain portions of `src/` own pure game types and
deterministic behavior.

Network command validation and server orchestration are application/adapter
boundaries; they must not bypass the world command pipeline.

Future persistence and client/network ports must be explicit interfaces.

`src/` adapter implementations own sockets and filesystem access.

`src/main.cpp` owns the current CLI entry point; a server entry point and
client delivery layer are planned by LISS-0056 and LISS-0064.

## Forbidden Placement

Do not put business rules in:

- UI component files.
- delivery/request handlers.
- adapters.
- persistence structs.
- provider SDK clients.

Do not put framework, file-system, network, or database imports in:

- `core/domain/`.
- `core/application/`, except through core-owned ports.

## Creation Rule

Create the smallest directory and module set needed by the current AT-TDD phase.
Do not scaffold unused adapters, providers, or UI features.

Smallest does not mean densest. Split files and modules when it reduces human
review cost or separates architectural responsibilities. Do not split only to
create speculative layers.

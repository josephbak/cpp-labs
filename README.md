# cpp-labs

Hands-on C++ labs for systems and compiler engineering. Each lab is a small, focused project that builds real intuition for performance, memory, and intermediate representations.

## Why this exists
- Learn **modern C++** (C++20) in **small, production-style units**.
- Practice skills needed for **MLIR (Multi-Level Intermediate Representation)**, **LLVM (Low Level Virtual Machine)**, and **IREE (Intermediate Representation Execution Environment)** work.
- Build tooling habits (CMake (Cross-Platform Make), sanitizers, tests) that transfer to real codebases.

## Layout
```
cpp-labs/
  lab01_smallvec_arena/    # Tiny bump allocator + small-vector container
  (more labs will follow…)
```

## Prerequisites (macOS)
- Apple Command Line Tools: `xcode-select --install`
- Homebrew: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
- Tooling: `brew install llvm cmake ninja`

> Uses Clang (C Language) from Command Line Tools by default. Ninja (Ninja Build System) recommended for fast builds. CMake (Cross-Platform Make) drives everything.

## Quick start
```bash
cd cpp-labs
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

### VS Code (Visual Studio Code) flow (recommended)
- Extensions: *C/C++*, *CMake Tools*.
- `CMake: Configure` → `CMake: Build` → `CMake: Run Tests`.

### Xcode (Xcode Integrated Development Environment) project (optional)
```bash
cmake -S . -B xbuild -G Xcode -DCMAKE_BUILD_TYPE=Debug
open xbuild/cpp-labs.xcodeproj
```

## Build types
- `Debug`: sanitizers on (Address/Undefined), easier debugging.
- `Release`: `-O3`, benchmarks only.

Switch with:
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Lab 01 — SmallVec + Arena
**Goal:** Implement an **Arena** (bump allocator) and a **SmallVec<T,N>** (small-buffer vector) that:
- Stores up to `N` elements inline; grows via **Arena** past `N`.
- Obeys RAII (Resource Acquisition Is Initialization) and Rule-of-Five (constructor, destructor, copy/move ctor, copy/move assign).
- Is validated with simple assertions and a smoke test.

**Why:** Teaches **memory layout**, **pointer arithmetic**, **construction/destruction**, and **allocation strategies** used in real compiler IRs (Intermediate Representations).

**Milestones:**
1. `Arena` with `allocate(bytes, alignment)` and `reset()`.
2. `SmallVec<T,N>` with push/pop and growth via `Arena`.
3. Basic tests and `ctest` wiring.
4. (Later) micro-bench vs `std::vector` for tiny sizes.

## Conventions
- Standard: **C++20**.
- Warnings as errors: `-Wall -Wextra -Wpedantic -Wconversion -Werror`.
- Sanitizers in Debug: `-fsanitize=address,undefined`.
- Style: keep headers minimal, prefer `.cpp` for implementations (templates may live in headers).

## FAQ (fast)
- **Why custom allocator?** Speed and determinism for short-lived objects (common in compilers).
- **Why small-vector?** Avoid heap for tiny sizes; improves cache locality and latency.
- **Why CMake (Cross-Platform Make)+Ninja (Ninja Build System)?** Deterministic, fast, industry-standard.

## Roadmap (next labs)
- **Lab 02:** Three-address IR + constant folding + dead-code elimination.
- **Lab 03:** SSA (Static Single Assignment) + CFG (Control Flow Graph) + dominance.
- **Lab 04:** MLIR (Multi-Level Intermediate Representation) toy dialect (op, canonicalization, lowering).
- **Lab 05:** Cache-aware GEMM (General Matrix Multiply) micro-kernel + benchmarks.

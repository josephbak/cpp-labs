# cpp-labs

A set of focused C++ labs oriented toward systems and AI compiler engineering. Each lab is self-contained, testable, and designed to build durable intuition for intermediate representations (IRs), control flow, memory management, and performance.

## Labs

| Lab | Title | Core Topics |
|------|--------|-------------|
| 1 | lab01_smallvec_arena | Arena allocator and small-buffer container fundamentals |
| 2 | lab02_three_address_ir | Three-address IR and passes (constant folding, dead-code elimination) |
| 3 | lab03_ssa_cfg | Control Flow Graph (CFG), dominators, SSA (Static Single Assignment) construction |
| 4 | lab04_mlir_toy_dialect | MLIR (Multi-Level Intermediate Representation) dialect: canonicalization and lowering to LLVM IR |
| 5 | lab05_gemm_kernel | Cache-aware GEMM (General Matrix Multiply) micro-kernel and benchmark |

## Prerequisites

- C++20 toolchain (Clang recommended)
- Debug build flags: `-Wall -Wextra -Wpedantic -Wconversion -Werror`
- Sanitizers: `-fsanitize=address,undefined`
- Optional: CMake + Ninja

## Build Example

```bash
clang++ -std=c++20 -O0 -g -fsanitize=address,undefined lab01_smallvec_arena/test_allocate.cpp -o /tmp/test_allocate
/tmp/test_allocate
```

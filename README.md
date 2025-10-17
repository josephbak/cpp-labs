# cpp-labs

A modular collection of focused C++ laboratories designed for systems programming and AI compiler engineering. Each lab is self-contained, testable, and emphasizes building durable intuition for intermediate representations (IRs), control flow, memory management, and performance optimization.

## Overview

The project progresses from foundational systems concepts to compiler-level abstractions and performance-tuned kernels. Every lab introduces a distinct layer of understanding relevant to modern compiler and runtime systems.

| Lab | Title | Core Topics |
|------|--------|-------------|
| 1 | lab01_smallvec_arena | Arena allocator design and small-buffer optimization techniques |
| 2 | lab02_three_address_ir | Three-address Intermediate Representation (IR), constant folding, and dead-code elimination |
| 3 | lab03_ssa_cfg | Control Flow Graph (CFG) construction, dominator analysis, and SSA (Static Single Assignment) form generation |
| 4 | lab04_mlir_toy_dialect | MLIR (Multi-Level Intermediate Representation) dialect development, canonicalization, and lowering to LLVM (Low-Level Virtual Machine) IR |
| 5 | lab05_gemm_kernel | Cache-aware GEMM (General Matrix Multiply) micro-kernel implementation and benchmarking |

## Prerequisites

- **Toolchain:** C++20 or later (Clang/LLVM recommended)
- **Build Flags:** `-Wall -Wextra -Wpedantic -Wconversion -Werror`
- **Sanitizers:** `-fsanitize=address,undefined`
- **Optional Tools:** CMake + Ninja for multi-lab builds

## Build Example

```bash
clang++ -std=c++20 -O0 -g -fsanitize=address,undefined \
  lab01_smallvec_arena/test_allocate.cpp -o /tmp/test_allocate
/tmp/test_allocate
```

## Project Goals

- Reinforce low-level understanding of allocation, ownership, and locality.
- Develop practical intuition for IR-based compiler transformations.
- Establish a continuous path from abstract program analysis to optimized hardware execution.

## Future Extensions

- Integrate cross-lab CMake configuration and automated testing.
- Add performance tracing utilities for kernel evaluation.
- Extend MLIR (Multi-Level Intermediate Representation) experiments toward hardware-aware dialects.

---

**Maintainer:** Joseph Bak  
**License:** MIT  
**Focus Areas:** Systems Programming, Compilers, AI Infrastructure


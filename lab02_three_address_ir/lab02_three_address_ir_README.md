# lab02_three_address_ir

A minimal linear Intermediate Representation (IR) for simple integer programs, featuring **constant folding**, **dead code elimination (DCE)**, and a **verifier** for enforcing definition-before-use correctness.

## Core Concepts

- **Three-Address IR:**  
  Each instruction has at most three operands (`dst`, `lhs`, `rhs`), representing `v_dst = op(v_lhs, v_rhs)` or `v_dst = const imm`.
- **Intermediate Representation (IR):**  
  An abstract, machine-independent layer for compiler analyses and transformations.
- **Constant Folding:**  
  Compile-time evaluation of operations whose inputs are constant.
- **Dead Code Elimination (DCE):**  
  Backward liveness analysis that removes unused, side-effect-free computations.
- **Verification Pass:**  
  Ensures use-before-def violations and malformed instructions are caught before optimization.

## Implementation Summary

| File | Purpose |
|------|----------|
| `ir.hpp / ir.cpp` | IR data model (`Instr`, `Program`), pretty-printer |
| `verify.cpp` | Checks instruction well-formedness (definition-before-use, field validity, return presence) |
| `passes_constfold.*` | Local constant folding and propagation |
| `passes_dce.*` | Backward liveness analysis and sweep |
| `main.cpp` | Driver: builds IR, verifies, runs passes, and prints transformations |

### Instruction Structure

```cpp
struct Instr {
    Op op;       // Const, Add, Sub, Mul, Ret
    int dst;     // virtual register defined by this instruction (unused for Ret)
    int lhs;     // left operand (or return value for Ret)
    int rhs;     // right operand (for binary ops)
    int64_t imm; // immediate constant (for Const)
};
```

### Example IR Sequence

```plaintext
v0 = consti64 2
v1 = consti64 3
v2 = addi64 v0, v1
v3 = muli64 v2, v2
v4 = consti64 0
ret v3
```

### After Optimization

```plaintext
v0 = consti64 2
v1 = consti64 3
v2 = consti64 5
v3 = consti64 25
ret v3
```

`v4` is eliminated by DCE (Dead Code Elimination).

## Learning Outcomes

- Understand how compilers represent programs using **three-address form**
- Implement **forward constant propagation and folding**
- Implement **backward liveness analysis** for DCE
- Enforce **definition-before-use** rule via static verification
- Observe pass sequencing in a minimal **optimization pipeline**

## Example Run

```bash
clang++ -std=c++20 -O0 -g -Wall -Wextra -Wpedantic -Wconversion -Werror         -fsanitize=address,undefined         main.cpp ir.cpp verify.cpp passes_constfold.cpp passes_dce.cpp -o /tmp/lab02_opt
/tmp/lab02_opt
```

Output:

```
=== input ===
v0 = consti64 2
v1 = consti64 3
v2 = addi64 v0, v1
v3 = muli64 v2, v2
v4 = consti64 0
ret v3

=== after constfold ===
v0 = consti64 2
v1 = consti64 3
v2 = consti64 5
v3 = consti64 25
v4 = consti64 0
ret v3

=== after dce ===
v0 = consti64 2
v1 = consti64 3
v2 = consti64 5
v3 = consti64 25
ret v3
```

## Next Steps

- Add `Op::Arg` to represent non-constant external inputs.  
- Extend DCE to track **side-effecting ops** (non-pure operations).  
- Prepare for **lab03_ssa_cfg**: lifting three-address IR into **SSA (Static Single Assignment)** form with CFG and φ-nodes.

---

### 🧩 Commit Message

```bash
git add lab02_three_address_ir/*
git commit -m "Add lab02_three_address_ir: minimal three-address IR with constant folding, DCE, and verifier"
```

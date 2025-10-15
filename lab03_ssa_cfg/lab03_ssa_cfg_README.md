# lab03_ssa_cfg

Control Flow Graph (CFG), **dominators**, **dominance frontiers (DF)**, and **φ-placement** (Cytron).  
This lab builds the foundation for Static Single Assignment (SSA) construction.  
We stop before SSA renaming; DF-based φ placement is implemented and verified.

> Convention: `dom[b][d] == true` means **d dominates b** (row = dominated, column = dominator).

---

## Status

- `ir.h` — defines `Function`, `Block`, and helper utilities.
- `cfg.{h,cpp}`  
  - `compute_dominators()` – iterative dataflow (fixed-point) dominator computation.  
  - `compute_idom()` – extracts **immediate dominators** correctly from the matrix.  
  - `compute_dominance_frontiers()` – Cytron DF via `idom`-chain climbs.  
- `ssa.{h,cpp}`  
  - `place_phi_blocks()` – Cytron worklist algorithm for φ-site placement.  
- `test_ssa.cpp` — builds test CFGs, prints CFG, `dom`, `idom`, `DF`, and φ-placement.

---

## Core definitions

**Control Flow Graph (CFG).**  
A directed graph $G=(V,E)$ where vertices are *basic blocks* and edges represent possible control transfers.

**Dominators.**  
A node $d$ *dominates* $b$ if every path from the entry to $b$ passes through $d$.  
Formally:
$$
Dom(b) = \{ d \mid d 	ext{ dominates } b \}
$$

**Immediate Dominator (idom).**  
For each non-entry block $b$, $idom(b)$ is the unique strict dominator $d \in Dom(b) \setminus \{b\}$ such that every other strict dominator $e$ of $b$ dominates $d$.  
Equivalently, $idom(b)$ is the *deepest* dominator of $b$ (its parent in the dominator tree).  
By convention, $idom(entry) = entry$.

**Dominance Frontier (DF).**  
For block $A$,
$$
DF(A) = \{ B \in V \mid \exists P \in preds(B): A \text{ dominates } P \land A \text{ does not strictly dominate } B \}
$$
Operationally (Cytron):  
For each block `B` and each `P ∈ preds(B)`, climb the `idom` chain from `P` up to but not including `idom[B]`, adding `B` to every visited node’s frontier.

**Φ-placement (Cytron worklist).**  
For each variable `x` with definition sites `defsites[x]`:
1. Initialize a worklist with `defsites[x]`.  
2. Pop a block `n`; for each `y ∈ DF(n)`, if `y` has no φ for `x`, add one and push `y` into the worklist (φ is a new def).  
Repeat until no new φ sites appear.

---

## Algorithms implemented

**1. Dominators (iterative dataflow).**  
Initialize `Dom(entry) = {entry}`, `Dom(b≠entry) = V`, then iterate
$$
Dom(b) = \{ b \} \cup \bigcap_{p \in preds(b)} Dom(p)
$$
until fixed point. This is a standard monotone dataflow iteration on a finite lattice.

**2. Immediate Dominators.**  
For each $b \ne entry$, choose $d \in Dom(b) \setminus \{b\}$ such that  
`dom[d][e] == true` for every other strict dominator $e$ of $b$.  
That `d` is the deepest dominator (the tree parent).

**3. Dominance Frontiers (Cytron).**
```cpp
for (int b = 0; b < N; ++b)
  for (int p : preds[b]) {
    int runner = p;
    while (runner != idom[b]) {
      DF[runner].push_back(b);
      runner = idom[runner];
    }
  }
```

**4. φ-placement.**  
Given `DF` and `defsites`, apply the Cytron worklist as described above.

---

## Worked examples

### Loop with header and exit

CFG:
```
0 → 1
1 → 2, 4
2 → 3
3 → 1   (backedge)
4 → 5
5 → (exit)
```
`preds(1)={0,3}` is the only multi-pred join.

Results:
- Dominators:  
  `Dom(0)={0}`, `Dom(1)={0,1}`, `Dom(2)={0,1,2}`, `Dom(3)={0,1,2,3}`, `Dom(4)={0,1,4}`, `Dom(5)={0,1,4,5}`.  
- `idom = [0,0,1,2,1,4]`.  
- DF: `DF[1]={1}`, `DF[2]={1}`, `DF[3]={1}` (others empty).  
- With `defsites[x]={0,3}`, φ for `x` appears at block 1 (loop header).

### Diamond (if-then-join)
```
0 → 1, 2
1 → 3
2 → 3
3 → exit
```
- Dominators: `Dom(0)={0}`, `Dom(1)={0,1}`, `Dom(2)={0,2}`, `Dom(3)={0,3}`.  
- `idom=[0,0,0,0]`.  
- DF: `DF[1]={3}`, `DF[2]={3}` → φ at join 3.

---

## Build

```bash
clang++ -std=c++20 -O0 -g   lab03_ssa_cfg/test_ssa.cpp   lab03_ssa_cfg/cfg.cpp   lab03_ssa_cfg/ssa.cpp   -o /tmp/test_ssa
/tmp/test_ssa
```

---

## Next steps
1. Build `dom_tree_children` from `idom[]` and verify preorder traversal.  
2. Implement SSA renaming (version stacks).  
3. Insert actual φ instructions; verify SSA invariant ∀v, |Defs(v)|=1.  
4. Add lit-style tests for diamonds and loops.

---

**References**
- Cytron et al., *Efficiently Computing Static Single Assignment Form and the Control Dependence Graph*, *TOPLAS* 1991.  
- Cooper, Harvey, Kennedy, *A Simple, Fast Dominance Algorithm*, *Software—Practice & Experience* 2001.

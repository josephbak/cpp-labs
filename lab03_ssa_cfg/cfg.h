#pragma once
#include "ir.h"
#include <vector>

// Computes dominator matrix dom[b][d] == true  ⇔  d dominates b
std::vector<std::vector<bool>> compute_dominators(const Function& F);

// Extracts immediate dominator for each block from dominator matrix.
// Returns idom[b] = parent in dominator tree; for entry, idom[entry] = entry.
// For unreachable blocks (only self-dominated), returns -1.
std::vector<int> compute_idom(const std::vector<std::vector<bool>>& dom, BlockId entry);

// Dominance frontiers: DF[u] is the list of blocks in the dominance frontier of u.
std::vector<std::vector<int>> compute_dominance_frontiers(const Function& F,
                                                          const std::vector<int>& idom);


// Build dominator tree adjacency from idom[].
// Returns: children[p] = vector of blocks immediately dominated by p.
// Assumes idom[entry] == entry; unreachable blocks may have idom[b] == -1 and are skipped.
std::vector<std::vector<int>> build_dom_tree_children(const std::vector<int>& idom);
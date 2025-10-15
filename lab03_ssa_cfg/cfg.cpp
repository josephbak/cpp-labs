#include "ir.h"
#include "cfg.h"
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>

// Dom matrix: dom[b][d] == true  ⇔  d dominates b
std::vector<std::vector<bool>> compute_dominators(const Function& F) {
    const int n = static_cast<int>(F.blocks.size());
    assert(n > 0);

    std::vector<std::vector<bool>> dom(n, std::vector<bool>(n, true));

    // Init: Dom(entry) = {entry}; Dom(b≠entry) = V (all true initially)
    for (int d = 0; d < n; ++d) dom[F.entry][d] = false;
    dom[F.entry][F.entry] = true;

    bool changed = true;
    while (changed) {
        changed = false;

        for (int b = 0; b < n; ++b) {
            if (b == F.entry) continue;

            // new_dom = {b} ∪ (⋂_{p∈preds(b)} Dom(p))
            std::vector<bool> new_dom(n, true);

            // Intersect dominator sets of all predecessors
            if (F.blocks[b].preds.empty()) {
                // Unreachable: by convention, only self-dominates
                for (int d = 0; d < n; ++d) new_dom[d] = false;
            } else {
                for (int d = 0; d < n; ++d) {
                    for (auto p : F.blocks[b].preds) {
                        if (!dom[p][d]) { new_dom[d] = false; break; }
                    }
                }
            }
            new_dom[b] = true; // a node always dominates itself

            // Check for change
            if (new_dom != dom[b]) {
                dom[b] = std::move(new_dom);
                changed = true;
            }
        }
    }

    return dom;
}

// dom[b][d] == true  ⇔  d dominates b
// idom[b] == immediate dominator (the deepest strict dominator of b)
std::vector<int> compute_idom(const std::vector<std::vector<bool>>& dom, BlockId entry) {
    const int n = static_cast<int>(dom.size());
    std::vector<int> idom(n, -1);

    // Entry dominates itself by convention
    idom[entry] = entry;

    for (int b = 0; b < n; ++b) {
        if (b == entry) continue;

        int candidate = -1;

        // Iterate over all strict dominators of b
        for (int d = 0; d < n; ++d) {
            if (d == b) continue;
            if (!dom[b][d]) continue; // d does not dominate b

            bool deepest = true;

            // Check whether d is dominated by all other strict dominators e of b
            for (int e = 0; e < n; ++e) {
                if (e == b || e == d) continue;
                if (!dom[b][e]) continue; // e is not a dominator of b

                // dom[d][e] == true → e dominates d
                // we need every e to dominate d
                if (!dom[d][e]) { // if some e does NOT dominate d
                    deepest = false;
                    break;
                }
            }

            if (deepest) {
                candidate = d;
                break;
            }
        }

        idom[b] = candidate; // may remain -1 for unreachable blocks
    }

    return idom;
}

std::vector<std::vector<int>>
compute_dominance_frontiers(const Function& F, const std::vector<int>& idom) {
    const int n = static_cast<int>(F.blocks.size());
    std::vector<std::vector<int>> DF(n);

    // Helper to append with dedup (DF is a set conceptually)
    auto insert_unique = [&](int u, int b) {
        auto& v = DF[u];
        if (std::find(v.begin(), v.end(), b) == v.end()) v.push_back(b);
    };

    for (int b = 0; b < n; ++b) {
        // For each predecessor p of b, climb the idom chain p → idom[p] → ... until idom[b].
        for (int p : F.blocks[b].preds) {
            int runner = p;
            while (runner != idom[b] && runner != -1) {
                insert_unique(runner, b);
                runner = idom[runner];
            }
        }
    }
    return DF;
}


std::vector<std::vector<int>> build_dom_tree_children(const std::vector<int>& idom) {
    const int n = static_cast<int>(idom.size());
    std::vector<std::vector<int>> children(n);
    for (int b = 0; b < n; ++b) {
        int p = idom[b];
        if (p < 0) continue;        // skip unreachable
        if (p == b) continue;       // entry: root has no parent
        children[p].push_back(b);   // p immediately dominates b
    }
    return children;
}
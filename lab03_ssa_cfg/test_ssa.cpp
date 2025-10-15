// lab03_ssa_cfg/test_ssa.cpp
#include <iostream>
#include <vector>
#include <utility>
#include "ir.h"
#include "cfg.h"
#include "ssa.h"

static void print_cfg(const Function& F) {
    for (int b = 0; b < (int)F.blocks.size(); ++b) {
        std::cout << "Block " << b << ":\n  succs: ";
        for (int s : F.blocks[b].succs) std::cout << s << " ";
        std::cout << "\n  preds: ";
        for (int p : F.blocks[b].preds) std::cout << p << " ";
        std::cout << "\n";
    }
}

static void print_dom_matrix(const std::vector<std::vector<bool>>& dom) {
    std::cout << "\nDominators (dom[b][d] = 1 if d dominates b):\n";
    for (int b = 0; b < (int)dom.size(); ++b) {
        std::cout << "b" << b << ": ";
        for (bool bit : dom[b]) std::cout << (bit ? 1 : 0) << " ";
        std::cout << "\n";
    }
}

static void print_idom(const std::vector<int>& idom) {
    std::cout << "\nidom (parent in dominator tree):\n";
    for (int b = 0; b < (int)idom.size(); ++b)
        std::cout << "idom[" << b << "] = " << idom[b] << "\n";
}

static void print_df(const std::vector<std::vector<int>>& DF) {
    std::cout << "\nDominance Frontiers:\n";
    for (int u = 0; u < (int)DF.size(); ++u) {
        std::cout << "DF[" << u << "]: ";
        for (int b : DF[u]) std::cout << b << " ";
        std::cout << "\n";
    }
}

static void print_dom_children(const std::vector<std::vector<int>>& kids) {
    std::cout << "\nDominator tree children:\n";
    for (int p = 0; p < (int)kids.size(); ++p) {
        std::cout << p << ": ";
        for (int c : kids[p]) std::cout << c << " ";
        std::cout << "\n";
    }
}

int main() {
    // === Build loop CFG ===
    // 0 → 1
    // 1 → 2, 4
    // 2 → 3
    // 3 → 1   (backedge)
    // 4 → 5
    // 5 → (exit)
    Function F;
    F.blocks.resize(6);
    auto add_edge = [&](int a, int b) {
        F.blocks[a].succs.push_back(b);
        F.blocks[b].preds.push_back(a);
    };
    add_edge(0,1);
    add_edge(1,2); add_edge(1,4);
    add_edge(2,3);
    add_edge(3,1); // backedge
    add_edge(4,5);

    std::cout << "=== CFG ===\n";
    print_cfg(F);

    // === Dominators, idom, DF ===
    auto dom  = compute_dominators(F);
    print_dom_matrix(dom);

    auto idom = compute_idom(dom, /*entry=*/0);
    print_idom(idom);

    auto DF   = compute_dominance_frontiers(F, idom);
    print_df(DF);

    auto dom_children = build_dom_tree_children(idom);
    print_dom_children(dom_children);

    // === φ-placement for a single variable v=0 ===
    // defsites[0] = {0, 3}  (initialized before loop, updated in loop body)
    std::vector<std::vector<int>> defsites(1);
    defsites[0] = {0, 3};

    auto phi_blocks = place_phi_blocks(DF, defsites);
    std::cout << "\nPhi placement (per variable):\n";
    for (int v = 0; v < (int)phi_blocks.size(); ++v) {
        std::cout << "var " << v << " → φ at blocks: ";
        for (int b : phi_blocks[v]) std::cout << b << " ";
        std::cout << "\n";
    }

    // === Prepare a tiny RenameInput to exercise the renamer ===
    RenameInput rin;
    rin.entry      = 0;
    rin.num_blocks = (int)F.blocks.size();
    rin.num_vars   = 1; // just variable 0
    rin.dom_children = dom_children;

    // preds/succs for SSA renamer
    rin.preds.resize(rin.num_blocks);
    rin.succs.resize(rin.num_blocks);
    for (int b = 0; b < rin.num_blocks; ++b) {
        rin.preds[b] = F.blocks[b].preds;
        rin.succs[b] = F.blocks[b].succs;
    }

    // Block-local defs/uses of var 0:
    // Defs: block 0 (init), block 3 (update inside loop)
    rin.block_defs.resize(rin.num_blocks);
    rin.block_defs[0] = {0};
    rin.block_defs[3] = {0};

    // Uses: sprinkle uses so we can see versioning propagate
    rin.block_uses.resize(rin.num_blocks);
    rin.block_uses[1] = {0}; // use at header (should see φ-version after renaming)
    rin.block_uses[2] = {0};
    rin.block_uses[3] = {0}; // use before redef at 3 will see header version
    rin.block_uses[4] = {0};
    rin.block_uses[5] = {0};

    rin.phi_blocks = phi_blocks; // from Cytron placement

    // === Run SSA renamer ===
    auto rout = rename_ssa(rin);

    // === Print renamer outputs ===
    std::cout << "\n=== SSA Rename Output ===\n";
    for (int b = 0; b < rin.num_blocks; ++b) {
        std::cout << "Block " << b << ":\n";

        // φ defs at this block
        if (!rout.phi_defs[b].empty()) {
            std::cout << "  phi_defs: ";
            for (const auto& pd : rout.phi_defs[b]) {
                std::cout << "v" << pd.var << "_" << pd.version << "  ";
            }
            std::cout << "\n";
        }

        // defs in this block
        if (!rout.def_versions[b].empty()) {
            std::cout << "  defs: ";
            for (auto [v,ver] : rout.def_versions[b]) {
                std::cout << "v" << v << "_" << ver << "  ";
            }
            std::cout << "\n";
        }

        // uses in this block
        if (!rout.use_versions[b].empty()) {
            std::cout << "  uses: ";
            for (auto [v,ver] : rout.use_versions[b]) {
                std::cout << "v" << v << "_" << ver << "  ";
            }
            std::cout << "\n";
        }
    }

    // Print φ incoming operands (wired per predecessor)
    std::cout << "\n=== Φ incoming operands ===\n";
    for (int b = 0; b < rin.num_blocks; ++b) {
        if (rout.phi_inputs[b].empty()) continue;
        std::cout << "Block " << b << " (phi inputs): ";
        for (const auto& pi : rout.phi_inputs[b]) {
            std::cout << "[from pred " << pi.pred << ": v" << pi.var << "_" << pi.version << "]  ";
        }
        std::cout << "\n";
    }

    return 0;
}

#include <iostream>
#include "ir.h"
#include "cfg.h"
#include "ssa.h"


static void print_cfg(const Function& f) {
    for (BlockId b = 0; b < (int)f.blocks.size(); ++b) {
        std::cout << "Block " << b << ":\n  succs: ";
        for (auto s : f.blocks[b].succs) std::cout << s << " ";
        std::cout << "\n  preds: ";
        for (auto p : f.blocks[b].preds) std::cout << p << " ";
        std::cout << "\n";
    }
}

int main() {
    // Function F;
    // // Build 4 blocks: 0=entry, 1,2,3(join)
    // F.entry = 0;
    // F.blocks.resize(4);

    // // Edges: 0->1, 0->2, 1->3, 2->3 (diamond)
    // add_edge(F, 0, 1);
    // add_edge(F, 0, 2);
    // add_edge(F, 1, 3);
    // add_edge(F, 2, 3);
    Function F;
    F.blocks.resize(6);
    F.blocks[0].succs = {1};
    F.blocks[1].succs = {2,4};
    F.blocks[2].succs = {3};
    F.blocks[3].succs = {1};  // backedge
    F.blocks[4].succs = {5};
    F.blocks[5].succs = {};

    F.blocks[1].preds = {0,3};
    F.blocks[2].preds = {1};
    F.blocks[3].preds = {2};
    F.blocks[4].preds = {1};
    F.blocks[5].preds = {4};

    print_cfg(F);

    auto dom = compute_dominators(F);

    std::cout << "\nDominators (dom[b][d] = 1 if d dominates b):\n";
    for (int b = 0; b < (int)F.blocks.size(); ++b) {
        std::cout << "b" << b << ": ";
        for (int d = 0; d < (int)F.blocks.size(); ++d) {
            std::cout << (dom[b][d] ? "1" : "0");
            if (d + 1 < (int)F.blocks.size()) std::cout << " ";
        }
        std::cout << "\n";
    }


    auto idom = compute_idom(dom, F.entry);
    std::cout << "\nidom (parent in dominator tree):\n";
    for (int b = 0; b < (int)idom.size(); ++b) {
        std::cout << "idom[" << b << "] = " << idom[b] << "\n";
    }


    auto DF = compute_dominance_frontiers(F, idom);
    std::cout << "\nDominance Frontiers:\n";
    for (int u = 0; u < (int)DF.size(); ++u) {
        std::cout << "DF[" << u << "]: ";
        for (int b : DF[u]) std::cout << b << " ";
        std::cout << "\n";
    }


    // Example 1: Diamond-style φ: if x is defined in blocks 1 and 2, φ at 3.
    // (Use your original diamond CFG to test this case.)

    // Example 2: Loop header φ: x defined before loop and inside loop body → φ at header.
    // For your loop CFG: defs at {0, 3} should place φ at {1}.

    std::vector<std::vector<int>> defsites;

    // Single variable v=0, defined at blocks 0 and 3 (pre-loop init + loop body)
    defsites.resize(1);
    defsites[0] = {0, 3};

    auto phi_blocks = place_phi_blocks(DF, defsites);

    std::cout << "\nPhi placement (per variable):\n";
    for (int v = 0; v < (int)phi_blocks.size(); ++v) {
        std::cout << "var " << v << " → φ at blocks: ";
        for (int b : phi_blocks[v]) std::cout << b << " ";
        std::cout << "\n";
    }
}
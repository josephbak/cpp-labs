#pragma once
#include "ir.h"
#include <vector>

// defsites[v] = list of blocks that define variable v
// DF[u]       = dominance frontier list for block u
// Returns: phi_blocks[v] = list of blocks where a φ for v must be placed
std::vector<std::vector<int>>
place_phi_blocks(const std::vector<std::vector<int>>& DF,
                 const std::vector<std::vector<int>>& defsites);


// Input to SSA renamer
struct RenameInput {
    int entry = 0;                                   // entry block id
    int num_blocks = 0;                              // |B|
    int num_vars = 0;                                // number of scalar vars
    std::vector<std::vector<int>> dom_children;      // children[p] = blocks immediately dominated by p
    std::vector<std::vector<int>> preds;             // preds[b]
    std::vector<std::vector<int>> succs;             // succs[b]

    // Per block: list of variable ids defined/used (order preserved for testing/printing)
    std::vector<std::vector<int>> block_defs;        // block_defs[b] = {v,...}
    std::vector<std::vector<int>> block_uses;        // block_uses[b] = {v,...}

    // φ placement result: phi_blocks[v] = {blocks that need a φ for v}
    std::vector<std::vector<int>> phi_blocks;
};

// Outputs (purely for inspection/printing; adapt later to your IR)
struct PhiDef { int var; int version; };
struct PhiInput { int var; int pred; int version; };

struct RenameOutput {
    // For each block, the version assigned to each def/use in input order
    std::vector<std::vector<std::pair<int,int>>> def_versions; // [(var,ver), ...]
    std::vector<std::vector<std::pair<int,int>>> use_versions; // [(var,ver), ...]

    // φ results per block
    std::vector<std::vector<PhiDef>>   phi_defs;    // φ defines (var,version) at this block
    std::vector<std::vector<PhiInput>> phi_inputs;  // for each φ at block, incoming (var,pred,version)
};

// Top-level: compute SSA versions for defs/uses and populate φ defs/inputs.
// Requires: phi_blocks already computed (place_phi_blocks).
RenameOutput rename_ssa(const RenameInput& in);
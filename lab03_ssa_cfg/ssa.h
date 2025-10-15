#pragma once
#include "ir.h"
#include <vector>

// defsites[v] = list of blocks that define variable v
// DF[u]       = dominance frontier list for block u
// Returns: phi_blocks[v] = list of blocks where a φ for v must be placed
std::vector<std::vector<int>>
place_phi_blocks(const std::vector<std::vector<int>>& DF,
                 const std::vector<std::vector<int>>& defsites);

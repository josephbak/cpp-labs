#include "ssa.h"
#include <vector>
#include <queue>
#include <unordered_set>

static void insert_unique(std::vector<int>& vec, int x) {
    for (int y : vec) if (y == x) return;
    vec.push_back(x);
}

// Cytron et al. φ-placement (worklist form)
std::vector<std::vector<int>>
place_phi_blocks(const std::vector<std::vector<int>>& DF,
                 const std::vector<std::vector<int>>& defsites) {
    const int V = static_cast<int>(defsites.size());
    std::vector<std::vector<int>> phi_blocks(V);

    for (int v = 0; v < V; ++v) {
        // Worklist initialized with all definition sites of v
        std::queue<int> wl;
        for (int b : defsites[v]) wl.push(b);

        // Track where we've already placed φ and visited in WL
        std::unordered_set<int> has_phi;   // blocks that already got a φ for v
        std::unordered_set<int> visited;   // defs we've processed

        while (!wl.empty()) {
            int n = wl.front(); wl.pop();
            if (!visited.insert(n).second) continue;

            // For each block y in DF(n), ensure a φ for v exists
            for (int y : DF[n]) {
                if (!has_phi.count(y)) {
                    has_phi.insert(y);
                    insert_unique(phi_blocks[v], y);

                    // If v is not *already* a defining block y, we must
                    // treat y as a new definition site (φ defines v at y)
                    bool y_is_def = false;
                    for (int d : defsites[v]) if (d == y) { y_is_def = true; break; }
                    if (!y_is_def) {
                        wl.push(y);
                    }
                }
            }
        }
    }
    return phi_blocks;
}

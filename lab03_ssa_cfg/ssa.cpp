#include "ssa.h"
#include <vector>
#include <stack>
#include <queue>
#include <unordered_set>
#include <algorithm>


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


static std::vector<std::vector<int>>
build_phi_vars_at_block(int num_blocks, int num_vars,
                        const std::vector<std::vector<int>>& phi_blocks) {
    std::vector<std::vector<int>> phi_vars_at_block(num_blocks);
    for (int v = 0; v < num_vars; ++v)
        for (int b : phi_blocks[v]) phi_vars_at_block[b].push_back(v);
    return phi_vars_at_block;
}

struct RenameState {
    const RenameInput* in;
    RenameOutput* out;
    std::vector<std::vector<int>> phi_vars_at_block; // phi_vars_at_block[b] = {v,...}

    // One stack per variable; top() = current visible version of var
    std::vector<std::vector<int>> stack;             // use vector as stack for speed
    std::vector<int> next_ver;                       // next version id for each var

    // For popping on exit: record vars we pushed in this block
    std::vector<int> pushed_phi_vars;
    std::vector<int> pushed_def_vars;

    void push_version(int v) {
        int ver = ++next_ver[v];
        stack[v].push_back(ver);
    }
    void pop_version(int v) {
        stack[v].pop_back();
    }
    int top_version(int v) const {
        return stack[v].empty() ? 0 : stack[v].back(); // 0 = initial version
    }
};

static void dfs_rename(int b, RenameState& S) {
    const auto& in  = *S.in;
    auto& out       = *S.out;

    S.pushed_phi_vars.clear();
    S.pushed_def_vars.clear();

    // (1) φ definitions at block b: each φ defines its var with a fresh version
    for (int v : S.phi_vars_at_block[b]) {
        S.push_version(v);
        S.pushed_phi_vars.push_back(v);
        out.phi_defs[b].push_back(PhiDef{v, S.top_version(v)});
    }

    // (2) Real definitions in block b: assign fresh versions
    out.def_versions[b].reserve(in.block_defs[b].size());
    for (int v : in.block_defs[b]) {
        S.push_version(v);
        S.pushed_def_vars.push_back(v);
        out.def_versions[b].push_back({v, S.top_version(v)});
    }

    // (3) Uses in block b: read the current visible version (top of stack)
    out.use_versions[b].reserve(in.block_uses[b].size());
    for (int v : in.block_uses[b]) {
        out.use_versions[b].push_back({v, S.top_version(v)});
    }

    // (4) For each successor s, wire φ incoming from predecessor b
    for (int s : in.succs[b]) {
        for (int v : S.phi_vars_at_block[s]) {
            out.phi_inputs[s].push_back(PhiInput{v, b, S.top_version(v)});
        }
    }

    // (5) Recurse preorder into dominator-tree children
    for (int c : in.dom_children[b]) dfs_rename(c, S);

    // (6) Pop what we defined in this block (restore caller's environment)
    for (int v : S.pushed_def_vars) S.pop_version(v);
    for (int v : S.pushed_phi_vars) S.pop_version(v);
}

RenameOutput rename_ssa(const RenameInput& in) {
    RenameOutput out;
    out.def_versions.resize(in.num_blocks);
    out.use_versions.resize(in.num_blocks);
    out.phi_defs.resize(in.num_blocks);
    out.phi_inputs.resize(in.num_blocks);

    RenameState S;
    S.in  = &in;
    S.out = &out;
    S.phi_vars_at_block = build_phi_vars_at_block(in.num_blocks, in.num_vars, in.phi_blocks);
    S.stack.assign(in.num_vars, {});
    S.next_ver.assign(in.num_vars, 0);

    // Initialize all variables with version 0 "in scope" (optional).
    // If you prefer "uninitialized uses" to be detectable, skip this and assert on empty.
    for (int v = 0; v < in.num_vars; ++v) S.stack[v].push_back(0);

    dfs_rename(in.entry, S);
    return out;
}
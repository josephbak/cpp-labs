#include "ir.hpp"
#include <unordered_set>

// Backward liveness from `ret` over pure ops; drop unused defs.
// Side-effect model: only Ret has an effect; Const/Add/Sub/Mul are pure.
void run_dce(Program& prog) {
    std::unordered_set<int> live;
    // Seed from all returns (support multiple rets for now)
    for (const auto& ins : prog) {
        if (ins.op == Op::Ret) live.insert(ins.lhs);
    }
    // Propagate liveness backward
    for (int i = static_cast<int>(prog.size()) - 1; i >= 0; --i) {
        const auto& ins = prog[static_cast<std::size_t>(i)];
        switch (ins.op) {
            case Op::Const:
                if (live.count(ins.dst)) {
                    // definition is used; keep; nothing to add
                }
                break;
            case Op::Add: case Op::Sub: case Op::Mul:
                if (live.count(ins.dst)) {
                    live.insert(ins.lhs);
                    live.insert(ins.rhs);
                }
                break;
            case Op::Ret:
                // already seeded
                break;
        }
    }
    // Sweep: remove any instruction that defines a dead vreg (and is pure)
    Program out;
    out.reserve(prog.size());
    for (const auto& ins : prog) {
        bool keep = true;
        switch (ins.op) {
            case Op::Const:
                keep = live.count(ins.dst) != 0;
                break;
            case Op::Add: case Op::Sub: case Op::Mul:
                keep = live.count(ins.dst) != 0;
                break;
            case Op::Ret:
                keep = true;
                break;
        }
        if (keep) out.push_back(ins);
    }
    prog.swap(out);
}

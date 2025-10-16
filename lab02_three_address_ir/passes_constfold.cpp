#include "ir.hpp"
#include <unordered_map>

// Very simple local constant propagation + folding.
// Tracks defs of vregs that are Const and folds binary ops whose operands are Const.
void run_constfold(Program& prog) {
    std::unordered_map<int, int64_t> constval;
    for (auto& ins : prog) {
        switch (ins.op) {
            case Op::Const:
                constval[ins.dst] = ins.imm;
                break;
            case Op::Add: case Op::Sub: case Op::Mul: {
                auto itL = constval.find(ins.lhs);
                auto itR = constval.find(ins.rhs);
                if (itL != constval.end() && itR != constval.end()) {
                    int64_t a = itL->second, b = itR->second, r = 0;
                    if (ins.op == Op::Add) r = a + b;
                    else if (ins.op == Op::Sub) r = a - b;
                    else r = a * b;
                    // Mutate into a Const defining the same dst.
                    ins.op = Op::Const;
                    ins.rhs = -1;
                    ins.imm = r;
                    constval[ins.dst] = r;
                } else {
                    // If any operand is non-const, result becomes non-const.
                    constval.erase(ins.dst);
                }
                break;
            }
            case Op::Ret:
                // no change
                break;
        }
    }
}

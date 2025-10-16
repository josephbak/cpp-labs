#include "ir.hpp"
#include <unordered_set>
#include <string>
#include <cstddef>
#include <cstdio>

static bool unused_fields_ok(const Instr& ins) {
    switch (ins.op) {
        case Op::Const: return ins.lhs == -1 && ins.rhs == -1;
        case Op::Add:
        case Op::Sub:
        case Op::Mul:   return ins.imm == 0; // imm unused; we keep it 0
        case Op::Ret:   return ins.dst == -1 && ins.rhs == -1 && ins.imm == 0;
    }
    return false;
}

bool verify(const Program& prog, std::string& err) {
    std::unordered_set<int> defined;
    bool saw_ret = false;

    for (std::size_t i = 0; i < prog.size(); ++i) {
        const Instr& ins = prog[i];

        if (!unused_fields_ok(ins)) {
            char buf[128];
            std::snprintf(buf, sizeof(buf), "instr %zu: unused field(s) not cleared", i);
            err = buf;
            return false;
        }

        switch (ins.op) {
        case Op::Const: {
            if (ins.dst < 0) { err = "Const: dst must be >= 0"; return false; }
            if (defined.count(ins.dst)) { err = "Const: dst redefinition"; return false; }
            defined.insert(ins.dst);
            break;
        }
        case Op::Add:
        case Op::Sub:
        case Op::Mul: {
            if (ins.dst < 0) { err = "BinOp: dst must be >= 0"; return false; }
            if (defined.count(ins.dst)) { err = "BinOp: dst redefinition"; return false; }
            if (ins.lhs < 0 || ins.rhs < 0) { err = "BinOp: lhs/rhs must be >= 0"; return false; }
            if (!defined.count(ins.lhs) || !defined.count(ins.rhs)) {
                err = "BinOp: use-before-def (lhs/rhs)";
                return false;
            }
            defined.insert(ins.dst);
            break;
        }
        case Op::Ret: {
            saw_ret = true;
            if (ins.lhs < 0) { err = "Ret: lhs (return value) must be >= 0"; return false; }
            if (!defined.count(ins.lhs)) { err = "Ret: use-before-def"; return false; }
            break;
        }
        }
    }

    if (!saw_ret) { err = "Program has no ret"; return false; }
    return true;
}

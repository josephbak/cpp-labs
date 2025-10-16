#include "ir.hpp"
#include <cstdio>

static const char* opname(Op op) {
    switch (op) {
        case Op::Const: return "consti64";
        case Op::Add:   return "addi64";
        case Op::Sub:   return "subi64";
        case Op::Mul:   return "muli64";
        case Op::Ret:   return "ret";
    }
    return "?";
}

void print_program(const Program& prog) {
    for (const auto& ins : prog) {
        switch (ins.op) {
            case Op::Const:
                std::printf("v%d = %s %lld\n", ins.dst, opname(ins.op),
                            static_cast<long long>(ins.imm));
                break;
            case Op::Add: case Op::Sub: case Op::Mul:
                std::printf("v%d = %s v%d, v%d\n", ins.dst, opname(ins.op),
                            ins.lhs, ins.rhs);
                break;
            case Op::Ret:
                std::printf("ret v%d\n", ins.lhs);
                break;
        }
    }
}

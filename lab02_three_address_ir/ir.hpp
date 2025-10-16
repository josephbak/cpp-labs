#pragma once
#include <cstdint>
#include <string>
#include <vector>

// Opcode set (integer-only MVP)
enum class Op : uint8_t { Const, Add, Sub, Mul, Ret };

// Linear three-address instruction.
// Fields used by each op:
// - Const:  dst, imm
// - Add/Sub/Mul: dst, lhs, rhs
// - Ret:    lhs (value to return)
struct Instr {
    Op op{};
    int dst{-1};        // virtual register id defined by this instr (unused for Ret)
    int lhs{-1};        // vreg id (or input) for binary ops, or return value for Ret
    int rhs{-1};        // vreg id (for binary ops)
    int64_t imm{0};     // immediate (only for Const)
};

using Program = std::vector<Instr>;

// Pretty-printer
void print_program(const Program& prog);

// Returns true if program is well-formed; otherwise writes an error message.
bool verify(const Program& prog, std::string& err);


// Pass APIs
void run_constfold(Program& prog);  // local constant folding
void run_dce(Program& prog);        // dead code elimination (no side effects yet)

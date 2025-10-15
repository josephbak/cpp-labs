#pragma once
#include <vector>
#include <optional>
#include <cstdint>
#include <cassert>

using BlockId = int;
using ValueId = int;
using InstrId = int;

enum class Opcode : uint8_t {
    Const,    // result := constant literal (we'll store the literal later)
    Add, Sub, Mul, Div,
    Phi,      // result := φ(args...) at join
    Br,       // unconditional branch to one successor
    CBr,      // conditional branch: two successors (true/false)
    Ret,      // return (0 or 1 operand)
};

struct Instr {
    Opcode op{};
    std::vector<ValueId> operands;           // use-ids
    std::optional<ValueId> result;           // def-id if this op defines a value
};

struct Block {
    std::vector<Instr> instrs;
    std::vector<BlockId> preds;              // filled by CFG build
    std::vector<BlockId> succs;              // explicit edges (for Br/CBr)
};

struct Function {
    BlockId entry = 0;
    std::vector<Block> blocks;
};

// Helper: add a directed edge b -> s (updates preds/succs)
inline void add_edge(Function& f, BlockId b, BlockId s) {
    assert(b >= 0 && b < (int)f.blocks.size());
    assert(s >= 0 && s < (int)f.blocks.size());
    f.blocks[b].succs.push_back(s);
    f.blocks[s].preds.push_back(b);
}

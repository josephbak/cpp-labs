#include "ir.hpp"
#include <cstdio>
#include <string>

int main() {

    // good one
    Program p = {
        // v0 = 2; v1 = 3; v2 = v0 + v1; v3 = v2 * v2; v4 = 0 (dead); ret v3
        {Op::Const, /*dst*/0, -1, -1, 2},
        {Op::Const, /*dst*/1, -1, -1, 3},
        {Op::Add,   /*dst*/2, /*lhs*/0, /*rhs*/1, 0},
        {Op::Mul,   /*dst*/3, /*lhs*/2, /*rhs*/2, 0},
        {Op::Const, /*dst*/4, -1, -1, 0}, // dead
        {Op::Ret,   /*dst*/-1, /*lhs*/3, -1, 0},
    };

    std::string err;
    if (!verify(p, err)) {
        std::fprintf(stderr, "verify failed: %s\n", err.c_str());
        return 1;
    }

    std::puts("=== input ===");
    print_program(p);

    run_constfold(p);
    std::puts("\n=== after constfold ===");
    print_program(p);

    run_dce(p);
    std::puts("\n=== after dce ===");
    print_program(p);

    return 0;
}

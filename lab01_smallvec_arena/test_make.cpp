#include <iostream>
#include "arena.h"

struct Foo {
    double x;
    int y;
    Foo(double a, int b) : x(a), y(b) {
        std::cout << "Foo constructed (" << x << ", " << y << ")\n";
    }
    ~Foo() { std::cout << "Foo destroyed\n"; }
};

int main() {
    // test_make.cpp

    Arena arena(128);

    Foo* f1 = arena.make<Foo>(3.14, 42);
    Foo* f2 = arena.make<Foo>(2.71, 99);

    std::cout << "f1 at " << f1 << " | f2 at " << f2 << '\n';
    std::cout << "Arena used: " << arena.used() << " bytes\n";

    // Manual destruction (optional)
    f1->~Foo();
    f2->~Foo();

    arena.reset();
}
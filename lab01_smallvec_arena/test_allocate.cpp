#include <iostream>
#include "arena.h"

struct Foo {
    double x;
    int y;
};

int main() {
    // allocation + alignment test
    
    Arena arena(128);

    std::cout << "Arena capacity: " << arena.capacity() << " bytes\n";

    void* p1 = arena.allocate(sizeof(int), alignof(int));
    void* p2 = arena.allocate(sizeof(double), alignof(double));
    void* p3 = arena.allocate(sizeof(Foo), alignof(Foo));

    void* p4 = arena.allocate(sizeof(int), alignof(int));

    std::cout << "p1 (int):    " << p1 << '\n';
    std::cout << "p2 (double): " << p2 << '\n';
    std::cout << "p3 (Foo):    " << p3 << '\n';
    std::cout << "p4 (int):    " << p4 << '\n';
    std::cout << "Used: " << arena.used() << " bytes\n";

    // Validate alignment
    std::cout << "alignof(int): " << alignof(int) << ", address % align = "
              << (reinterpret_cast<std::uintptr_t>(p1) % alignof(int)) << '\n';
    std::cout << "alignof(double): " << alignof(double) << ", address % align = "
              << (reinterpret_cast<std::uintptr_t>(p2) % alignof(double)) << '\n';
    std::cout << "alignof(Foo): " << alignof(Foo) << ", address % align = "
              << (reinterpret_cast<std::uintptr_t>(p3) % alignof(Foo)) << '\n';
    std::cout << "alignof(int): " << alignof(int) << ", address % align = "
              << (reinterpret_cast<std::uintptr_t>(p4) % alignof(int)) << '\n';

    arena.reset();
    std::cout << "After reset(), used = " << arena.used() << '\n';
}

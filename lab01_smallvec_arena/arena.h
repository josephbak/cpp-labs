#include <cstddef>
#include <new>

#pragma once

class Arena{
    unsigned char* base_ = nullptr; // pointer to the start of the raw memory
    std::size_t capacity_ = 0;  // how big is it (in bytes)
    std::size_t offset_ = 0;    // where the next allocation begins

    public:
    // Constructor: called when you create an Arena
    explicit Arena(std::size_t bytes)
        // Member initializer list:
        : base_(new (std::nothrow) unsigned char[bytes]), // allocate raw bytes
          capacity_(bytes),                               // store total capacity
          offset_(0)                                      // start offset = 0
    {
        if (!base_) throw std::bad_alloc{}; // if allocation failed, throw error
    }

    // Destructor: called automatically when Arena goes out of scope
    ~Arena() {
        delete[] base_; // release the raw memory
    }

    


};
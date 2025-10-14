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

        static std::uintptr_t align_up(std::uintptr_t addr, std::size_t align) noexcept {
            // Aligns 'addr' to the next multiple of 'align'
            // Works because align is a power of two.
            return (addr + (align - 1)) & ~(align - 1);
        }

        void* allocate(std::size_t bytes, std::size_t align = alignof(std::max_align_t)) {
            std::uintptr_t curr = reinterpret_cast<std::uintptr_t>(base_) + offset_;
            std::uintptr_t aligned = align_up(curr, align);
            std::size_t new_off = (aligned - reinterpret_cast<std::uintptr_t>(base_)) + bytes;

            if (new_off > capacity_)
                throw std::bad_alloc{};

            offset_ = new_off;
            return reinterpret_cast<void*>(aligned);
        }

        template <typename T, typename... Args>
        T* make(Args&&... args) {
            void* mem = allocate(sizeof(T), alignof(T));
            return new (mem) T(std::forward<Args>(args)...);
        }

        void reset() { offset_ = 0; }

        // (optional) helpers
        std::size_t used() const { return offset_; }
        std::size_t capacity() const { return capacity_; }


};
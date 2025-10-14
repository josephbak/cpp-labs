# lab01_smallvec_arena

## Objective

Implement a **bump (arena) allocator** that performs aligned, O(1) sub-allocations from a contiguous buffer, and extend it with a typed construction helper `make<T>(args...)` using placement new.

## Concepts

### Alignment

Addresses must satisfy:

$$
\text{address} \bmod \text{alignof}(T) = 0
$$

Distinguish between:
- **Size** — number of bytes requested.
- **Alignment** — starting boundary constraint.

### Performance Model

- One heap allocation for the backing store.
- Sub-allocations via offset increments (O(1)).
- Bulk deallocation via `reset()`.

### Allocation Diagram

```
base_ → |-----used-----|------free------|
         0            offset          capacity
```

Allocation simply bumps the offset forward. Reset sets `offset = 0`.

## Files

| File | Description |
|-------|-------------|
| `arena.h` | Header-only bump allocator (`allocate`, `reset`, `make<T>`) |
| `test_allocate.cpp` | Tests raw allocation and alignment |
| `test_make.cpp` | Tests typed object construction and destruction |

## Build Commands

```bash
clang++ -std=c++20 -O0 -g -fsanitize=address,undefined lab01_smallvec_arena/test_allocate.cpp -o /tmp/test_allocate
/tmp/test_allocate

clang++ -std=c++20 -O0 -g -fsanitize=address,undefined lab01_smallvec_arena/test_make.cpp -o /tmp/test_make
/tmp/test_make
```

## Success Criteria

- All returned pointers satisfy their type alignment (address % align == 0).
- Sub-allocations execute in O(1) time.
- `reset()` reuses the entire arena memory.

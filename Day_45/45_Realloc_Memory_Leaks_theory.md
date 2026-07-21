# Day 45: Dynamic Memory Resizing with `realloc()` & Memory Leak Prevention

## 1. Overview & Real-World Analogy
`realloc()` (*re-allocation*) changes the size of a previously allocated memory block. It enables dynamic data structures (such as resizable dynamic arrays / vectors, string builders, and buffers) to grow or shrink at runtime to match exact payload needs.

### Real-World Analogy
Imagine an expanding warehouse:
- **Case 1 (In-Place Expansion)**: You own a warehouse and need 50% more space. Fortunately, the adjacent empty lot is available. The landlord extends your boundary fence without moving any of your stored goods.
- **Case 2 (Relocation & Copy)**: The adjacent lot is already occupied by someone else. The landlord rents you a brand new, larger warehouse across town, transports all your inventory to the new location, and sells off the old warehouse.
- **Case 3 (Eviction Failure / Leak)**: You ask for a mega-warehouse that does not exist. If you discard the keys to your current warehouse while waiting for the failed upgrade, you lose all your stored inventory forever!

---

## 2. Deep Technical Dive & Memory Dynamics

### Signature (`<stdlib.h>`)
```c
void *realloc(void *ptr, size_t new_size);
```

### Execution Pathways of `realloc(ptr, new_size)`:
1. **`ptr == NULL`**:
   - Behaves identically to `malloc(new_size)`.
2. **`new_size == 0` (and `ptr != NULL`)**:
   - In ISO C99/C11, this frees the block and returns either `NULL` or a non-dereferenceable pointer. *(Note: Deprecated in C23; use explicit `free(ptr)` instead).*
3. **In-Place Expansion (Contiguous Heap Space Available)**:
   - The allocator simply updates the chunk header metadata size.
   - The address returned is identical to `ptr`. No data is copied. Time complexity: $O(1)$.
4. **Relocation (Contiguous Space Blocked)**:
   - The allocator locates a new memory block of size $\ge \text{new\_size}$.
   - Copies $\min(\text{old\_size}, \text{new\_size})$ bytes from the old location to the new location.
   - Frees the old memory block automatically.
   - Returns the address of the newly allocated block. Time complexity: $O(N)$.
5. **Allocation Failure (Out of Memory)**:
   - Returns `NULL`.
   - **CRITICAL**: The original memory block at `ptr` remains **intact and valid**. It is **NOT** freed!

```
In-Place Expansion:
[ Header | Payload (16B) ] [ Free Chunk (32B) ]  ===>  [ Header | Payload (48B)         ]

Relocation:
Old Chunk: [ Header | Payload (16B) ] [ Allocated Block (Busy) ]
             |
             v (memcpy)
New Chunk: [ Header | Copy Payload (16B) + Extra (32B) ]
Old Chunk: [ Header | (FREED / Put back in bins)       ]
```

---

## 3. The Fatal `realloc` Pitfall & The Temporary Pointer Pattern

### The Anti-Pattern (Immediate Overwrite)
```c
// DANGEROUS BUG: Memory leak if realloc fails!
int *arr = malloc(100 * sizeof(int));
arr = realloc(arr, 2000000000 * sizeof(int)); // If realloc fails, returns NULL.
// Now arr is NULL, and the original 100 ints are permanently leaked!
```

### The Production Idiom (Temporary Pointer)
```c
int *arr = malloc(100 * sizeof(int));

// SAFE PATTERN:
int *temp = realloc(arr, new_size * sizeof(int));
if (temp == NULL) {
    // Handle OOM: 'arr' is still valid!
    free(arr);
    return ERROR_OOM;
}
arr = temp; // Safe assignment
```

---

## 4. Geometric vs. Arithmetic Capacity Growth

When implementing a dynamic array (vector):
- **Arithmetic Growth ($+K$ elements)**: Allocating $K$ elements every resize leads to $O(N^2)$ overall copying cost across $N$ insertions.
- **Geometric Growth ($\times 1.5$ or $\times 2.0$)**: Doubling the capacity guarantees an **amortized $O(1)$** insertion time per element.

```
Capacity Progression (Geometric x2):
1 -> 2 -> 4 -> 8 -> 16 -> 32 -> 64 -> 128 -> 256 ...
Total copies for N items = N + N/2 + N/4 + ... <= 2N ==> O(N) Total ==> O(1) Amortized
```

---

## 5. Side-by-Side Comparison: Memory Allocation Functions

| Function | Signature | Initial Contents | Resizes Existing Block? |
| :--- | :--- | :--- | :--- |
| `malloc(size)` | `void *malloc(size_t)` | Uninitialized garbage | No (New block) |
| `calloc(n, size)`| `void *calloc(size_t, size_t)` | Guaranteed zeroed | No (New block) |
| `realloc(ptr, size)`| `void *realloc(void *, size_t)`| Preserves old data; new bytes garbage | Yes (In-place or migrated) |
| `free(ptr)` | `void free(void *)` | Deallocated / Invalid | N/A (Release) |

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Retaining Old Pointers after Relocation**:
   Storing pointers to elements inside a reallocated buffer leads to dangling pointers if `realloc` relocates the base address.
2. **Accessing Uninitialized Extension Memory**:
   `realloc` does **not** zero out the newly expanded region; new memory contains garbage data.
3. **Double Free on Error**:
   Calling `free(ptr)` after attempting `ptr = realloc(ptr, size)` when it returned `NULL` might accidentally free a `NULL` pointer while leaking the block if pointers got mixed.

---

## 7. Complexity & Production Best Practices

- **Time Complexity**:
  - In-place: $O(1)$
  - Relocated: $O(N)$ where $N$ is the copied byte count.
  - Amortized dynamic array insertion with doubling: $O(1)$.
- **Space Overhead**: Heap chunk metadata + temporary memory during relocation.
- **Valgrind / ASan**: Always test dynamic resizing buffers against memory leak analyzers (`valgrind --leak-check=full` or AddressSanitizer).

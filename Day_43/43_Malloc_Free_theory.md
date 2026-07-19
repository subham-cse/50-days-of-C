# Day 43: Dynamic Memory Allocation with `malloc()` and `free()`

## 1. Overview & Real-World Analogy
C programs operate with several distinct memory regions: **Text** (code binary), **Data/BSS** (global/static variables), **Stack** (local variables, frame pointers), and the **Heap** (dynamic runtime allocations).

`malloc()` (*memory allocation*) requests a contiguous block of uninitialized bytes from the heap. `free()` returns that block back to the allocator's management pool.

### Real-World Analogy
Think of the Heap as a hotel room booking system.
- Calling `malloc(sizeof(Room))` books an available room. The concierge hands you a room key (`pointer`). The room is uncleaned—it contains whatever the previous guest left behind (**uninitialized garbage data**).
- Calling `free(room_key)` checks out of the room. The concierge marks the room as available for future guests.
- If you never check out (**Memory Leak**), the hotel eventually runs out of rooms.
- If you check out twice (**Double Free**), the hotel records get corrupted.
- If you sneak into the room after checking out (**Use-After-Free**), you might crash into a new guest.

---

## 2. Deep Technical Dive & Memory Architecture

```
High Memory Addresses (0x7FFFFFFFFFFF)
+-------------------------------------------------------------+
| Stack (Grows Downward | Local Vars, Return Addresses)       |
|   |                                                         |
|   v                                                         |
|                                                             |
|   ^                                                         |
|   |                                                         |
| Heap (Grows Upward via brk/sbrk or mapped via mmap)         |
+-------------------------------------------------------------+
| BSS Segment (Uninitialized Globals / Statics - Zeroed)      |
+-------------------------------------------------------------+
| Data Segment (Initialized Globals / Statics)                |
+-------------------------------------------------------------+
| Text Segment (Read-only Executable Machine Code)            |
+-------------------------------------------------------------+
Low Memory Addresses (0x000000000000)
```

### Heap Chunk Anatomy (e.g., glibc `ptmalloc`)
When `malloc(N)` is called, the allocator reserves extra bytes preceding the returned address to store bookkeeping metadata:

```
+------------------------------------+------------------------------------------+
| Chunk Header (Size + Status Flags) | Usable User Payload (Returned by malloc) |
| 8 or 16 bytes metadata             | N bytes requested (aligned to 8/16 bytes)|
+------------------------------------+------------------------------------------+
^                                    ^
| Actual Allocated Block Start       | Pointer returned to caller
```

- **Header Size**: Contains the total chunk size and flags (e.g., `PREV_INUSE`, `IS_MMAPPED`).
- **Alignment**: Standard C allocators guarantee memory aligned for the most demanding scalar type (usually 8 bytes on 32-bit, 16 bytes on 64-bit).

---

## 3. Syntax & Signatures

Defined in `<stdlib.h>`:

```c
void *malloc(size_t size);
void free(void *ptr);
```

- **Parameters**: `size` is the number of contiguous bytes to allocate.
- **Return Value**: Returns a pointer of type `void *` to the allocated space, or `NULL` if allocation fails (e.g., system out of memory or request exceeds address space limits).
- **Behavior of `free(NULL)`**: The C standard explicitly guarantees that calling `free(NULL)` is a safe no-op.

---

## 4. Under the Hood: System Calls (`brk`, `sbrk`, `mmap`)

The C runtime library does not issue a kernel system call for every single `malloc()` invocation. Doing so would be catastrophic for performance.
1. **Small / Medium Allocations (< 128 KB default threshold)**:
   - The allocator uses `brk()` / `sbrk()` to advance the process break boundary.
   - It maintains internal **bins / free-lists** (Fastbins, Smallbins, Unsorted bins) to rapidly recycle freed memory blocks in user-space without kernel context switches.
2. **Large Allocations (>= 128 KB)**:
   - The allocator issues an `mmap()` syscall to map anonymous virtual memory pages directly from the kernel.
   - When `free()` is called on an mmapped chunk, `munmap()` immediately returns the pages to the OS.

---

## 5. Memory Allocation Flow Chart

```
+----------------------------------+
|      Caller requests malloc(N)   |
+----------------------------------+
                 |
                 v
      +----------------------+
      | Fits in Free List?   |--- YES ---> [Pop chunk & return pointer]
      +----------------------+
                 | NO
                 v
      +----------------------+
      | Size >= MMAP_THRESH? |--- YES ---> [Syscall mmap() & return pointer]
      +----------------------+
                 | NO
                 v
      +----------------------+
      | Extend heap via brk  |-----------> [Carve block & return pointer]
      +----------------------+
```

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Failure to Check for `NULL`**:
   Dereferencing a pointer returned by `malloc()` without validating `ptr != NULL` causes crashes when out of memory.
2. **Memory Leaks**:
   Losing all references to allocated memory before invoking `free()`.
3. **Buffer Overrun / Out-of-bounds Write**:
   Writing beyond the requested `size` corrupts adjacent chunk headers in the heap, causing allocator crashes on subsequent `malloc`/`free` calls.
4. **Casting `malloc()` in C**:
   In C, `int *p = malloc(...)` is preferred over `int *p = (int *)malloc(...)`. Casting can mask the failure to include `<stdlib.h>` in older standards.

---

## 7. Complexity & Production Best Practices

- **Time Complexity**:
  - `malloc()`: $O(1)$ amortized average case; $O(N)$ worst-case free-list traversal.
  - `free()`: $O(1)$ constant time bin insertion & coalescing.
- **Space Overhead**: Heap chunks incur an 8–16 byte metadata overhead per allocation.
- **Rule of Thumb**: Allocate once for structured batches rather than millions of tiny allocations to reduce fragmentation and metadata overhead.

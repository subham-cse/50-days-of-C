# Day 36: Structures — Basics, Memory Alignment, Padding & `offsetof`

## 1. Title & Overview
A **Structure (`struct`)** in C is a user-defined composite data type that groups related variables of potentially different types under a single contiguous block of memory.

While members within a struct are laid out sequentially in memory, their physical addresses rarely equal the simple sum of their individual sizes due to **Hardware Memory Alignment** and **Compiler Padding**.

### Real-World Analogy: Cargo Pallet Packing
Imagine packing goods onto standardized 8-foot shipping pallets:
- If you place a 1-foot box (`char`), followed by an 8-foot engine block (`double`), the engine block cannot start in the middle of a pallet boundary. 
- You must insert 7 feet of empty packing foam (**padding bytes**) to align the heavy engine block onto the next clean 8-foot pallet.
- If you reorganize your cargo by placing the largest items first, you eliminate wasted packing foam.

---

## 2. Deep Technical Dive: Memory Alignment Rules

### 2.1 Natural Alignment Requirements
Microprocessors (x86-64, ARM, RISC-V) fetch data from system RAM over 32-bit or 64-bit data buses. Accessing a memory address aligned to an integer multiple of the data type's size requires a single bus cycle:
- `char` (1 byte): Any address ($A \pmod 1 = 0$).
- `short` (2 bytes): Multiple of 2 ($A \pmod 2 = 0$).
- `int`, `float` (4 bytes): Multiple of 4 ($A \pmod 4 = 0$).
- `double`, `uint64_t`, pointers (8 bytes): Multiple of 8 ($A \pmod 8 = 0$).

### 2.2 Internal & Tail Padding
1. **Internal Padding**: The compiler automatically inserts unused padding bytes before a member if the current byte offset is not divisible by that member's required alignment.
2. **Tail Padding (Overall Alignment)**: The total size of a struct (`sizeof`) is rounded up to a multiple of its **largest single member alignment**. This guarantees that in an array of structures, every subsequent element begins at a properly aligned memory address.

### 2.3 The `offsetof` Macro
Defined in `<stddef.h>`, `offsetof(type, member)` returns the exact offset in bytes from the start of the struct to the specified member:
$$\text{Offset} = \text{size\_t}(\&((\text{type} *)0)\text{->member})$$

---

## 3. ASCII Memory Layout & Padding Byte Maps

### Unoptimized Struct vs. Reordered Struct vs. Packed Struct

#### Scenario A: Unoptimized Struct (`sizeof = 24 bytes`)
```c
struct Unoptimized {
    char   a;      /* 1 byte  */
    int    b;      /* 4 bytes */
    short  c;      /* 2 bytes */
    double d;      /* 8 bytes */
};
```
```text
Byte Offset:
00 01 02 03  04 05 06 07  08 09 10 11  12 13 14 15  16 17 18 19 20 21 22 23
+--+--------+-----------+-----+--------+-----------+-----------------------+
|a | [PAD 3]|     b     |  c  | [PAD 6]| (unused)  |           d           |
+--+--------+-----------+-----+--------+-----------+-----------------------+
|<- 4 bytes ->|<- 4 bytes ->|<-   8 bytes (pad 6) ->|<-      8 bytes      ->|
Total Size = 24 bytes (Wasted Padding = 9 bytes = 37.5% overhead!)
```

#### Scenario B: Optimized Struct (Descending Order: `sizeof = 16 bytes`)
```c
struct Optimized {
    double d;      /* 8 bytes */
    int    b;      /* 4 bytes */
    short  c;      /* 2 bytes */
    char   a;      /* 1 byte  */
};
```
```text
Byte Offset:
00 01 02 03 04 05 06 07  08 09 10 11  12 13  14  15
+-----------------------+-----------+-----+---+----+
|           d           |     b     |  c  | a |[P1]|
+-----------------------+-----------+-----+---+----+
|<-      8 bytes      ->|<- 4 bytes->| 2B  | 1B| 1B | (Tail Pad = 1 byte)
Total Size = 16 bytes (Saved 8 bytes per struct instance!)
```

#### Scenario C: Packed Struct (`#pragma pack(1)`: `sizeof = 15 bytes`)
```text
Byte Offset:
00 01 02 03 04  05 06  07 08 09 10 11 12 13 14
+--+-----------+-----+-----------------------+
|a |     b     |  c  |           d           |
+--+-----------+-----+-----------------------+
Total Size = 15 bytes (0 padding bytes; unaligned hardware access penalty)
```

---

## 4. Side-by-Side Comparison

| Metric | Unoptimized Struct | Reordered / Optimized | Packed (`#pragma pack(1)`) |
| :--- | :--- | :--- | :--- |
| **Total Memory Size** | 24 bytes | 16 bytes | 15 bytes |
| **Padding Overhead** | 9 bytes (37.5%) | 1 byte (6.25%) | 0 bytes (0%) |
| **CPU Read Performance** | Fast (Aligned) | **Fastest (Aligned & Compact)**| Slower (Unaligned memory cycles)|
| **Cache Line Efficiency**| Moderate | **High (More items per cache line)**| Highest density |
| **Network / Binary IO** | Unsafe across platforms | Unsafe across platforms | **Ideal for wire protocol serialization**|

---

## 5. Under the Hood: Hardware Bus Access & Cache Lines
- **Word Boundaries**: When a CPU reads a 4-byte `int` that straddles two 64-bit cache lines or two 32-bit bus words, the memory controller must issue **two read operations**, shift the bits, and combine them. On architectures like ARMv6 or SPARC, misaligned access causes a hardware alignment exception (`SIGBUS`).
- **Cache Alignment**: Placing frequently accessed members together at the top of the struct ensures they share the same L1 data cache line (typically 64 bytes).

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Comparing Structs with `memcmp()`**:
   ```c
   struct Data s1 = { .a = 'A', .b = 100 };
   struct Data s2 = { .a = 'A', .b = 100 };
   if (memcmp(&s1, &s2, sizeof(struct Data)) == 0) // BUG: Uninitialized padding bytes
   ```
   Padding bytes contain undefined garbage stack values. Direct byte comparison (`memcmp`) can fail even when all fields are identical. Compare member-by-member instead.
2. **Assuming Struct Size is Sum of Members**:
   Writing `sizeof(struct S)` assuming it equals $\sum \text{sizeof}(members)$ breaks buffer allocations and binary file parsing.
3. **Pointers to Unaligned Members in Packed Structs**:
   Passing the address of an unaligned member in a packed struct (`&packed_struct.d`) to a function expecting an aligned `double *` can trigger undefined behavior or crash on strict-alignment architectures.

---

## 7. Complexity & Production Best Practices
- **Design Rule**: Always arrange structure members in **descending order of size** (e.g., 64-bit pointers/doubles first, 32-bit ints, 16-bit shorts, 8-bit chars last).
- **Serialization**: For network packets and disk headers, use packed structs or serialize fields explicitly byte-by-byte using bitwise shifts.

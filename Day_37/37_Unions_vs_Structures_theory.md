# Day 37: Unions vs. Structures — Shared Memory & Tagged Unions

## 1. Title & Overview
While a **Structure (`struct`)** allocates distinct, non-overlapping memory slots for each of its members, a **Union (`union`)** allocates a single shared block of memory where **all members begin at the exact same base memory address (offset 0)**.

At any given instant in time, a union holds a valid value for **only one** of its declared members.

### Real-World Analogy: Multi-Room Apartment vs. Shared Single Bed
- **`struct` (Multi-Room Apartment)**: Every resident (member) has their own designated bedroom. Alice can sleep in Room 1 while Bob works in Room 2 simultaneously. The total square footage is the sum of all rooms (plus hallway padding).
- **`union` (Shared Single Bed)**: There is only one bed in the room. If Alice lies down, Bob must step out. If Bob climbs in, he overwrites Alice's position. The room size is simply the size required by the largest person.

---

## 2. Deep Technical Dive: Memory Semantics & Type Punning

### 2.1 Memory Size and Address Overlap
- **Base Address**: For any union instance `u`, `&u == (void*)&u.member_a == (void*)&u.member_b`.
- **Union Size**:
  $$\text{sizeof}(\text{union}) = \max(\text{sizeof}(m_1), \text{sizeof}(m_2), \dots) + \text{tail\_padding}$$
  The total size must be a multiple of the largest member's alignment requirement.

### 2.2 Tagged (Discriminated) Unions
Because raw unions do not store runtime metadata indicating which member is currently active, production C software encapsulates unions within a **Tagged Union** (or Discriminated Union):
```c
struct DynamicVariant {
    enum DataType type_tag; /* Identifies active payload */
    union {
        int64_t   as_int;
        double    as_float;
        char     *as_string;
    } payload;
};
```

### 2.3 Type Punning & C99 Semantics
**Type Punning** is the practice of writing data to one union member and reading it back through another member to inspect raw binary representation (e.g., extracting the exponent/mantissa bits of an IEEE-754 `float` via `uint32_t`).
- In **ANSI C99 / C11 (Section 6.5.2.3)**, type punning through a union is officially supported and is **not** an aliasing violation (unlike pointer casting `*(uint32_t*)&float_val` which violates the Strict Aliasing Rule).

---

## 3. ASCII Memory Layout: Struct vs. Union

```text
=============================================================================
 STRUCT MEMORY ALLOCATION (Sequential & Separate)
 struct ExampleStruct { char c; int i; double d; };  -> sizeof = 16 bytes
=============================================================================
 Offset: 00  01 02 03  04 05 06 07  08 09 10 11 12 13 14 15
        +---+--------+-----------+-----------------------+
        | c | [PAD 3]|     i     |           d           |
        +---+--------+-----------+-----------------------+
        <- 1B ->       <- 4 bytes->  <-      8 bytes      ->

=============================================================================
 UNION MEMORY ALLOCATION (Overlapping Shared Base Address: Offset 0)
 union ExampleUnion { char c; int i; double d; };    -> sizeof = 8 bytes
=============================================================================
 Offset: 00 01 02 03 04 05 06 07
        +-----------------------+
        | c (1 byte)            |  (&u.c == 0x1000)
        +-----------------------+
        | i (4 bytes)           |  (&u.i == 0x1000)
        +-----------------------+
        | d (8 bytes)           |  (&u.d == 0x1000)
        +-----------------------+
        |<------ 8 bytes ------>|
```

---

## 4. Side-by-Side Comparison

| Feature | Structure (`struct`) | Union (`union`) |
| :--- | :--- | :--- |
| **Memory Allocation** | Distinct slot for every member | Single shared overlapping slot |
| **Member Addresses** | Ascending distinct offsets | **All members have identical base address** |
| **Total Size** | $\ge \sum \text{sizeof}(m_i)$ (with padding) | $\max(\text{sizeof}(m_i))$ (with tail padding) |
| **Active Members** | All members coexist simultaneously | **Only ONE member active at any time** |
| **Member Modification**| Modifying one field leaves others intact | **Modifying one member overwrites all others** |
| **Primary Use Cases** | Grouping heterogeneous records (Person, Packet)| Polymorphism, variants, hardware registers, type punning |

---

## 5. Under the Hood: Endianness Detection & Bit Inspection
Unions provide a zero-overhead hardware mechanism for detecting CPU byte ordering (**Endianness**):
```c
union EndianCheck {
    uint32_t word;
    uint8_t  bytes[4];
};
```
If `word = 0x01020304` is written:
- **Little-Endian (x86, ARM)**: Least Significant Byte stored first $\implies$ `bytes[0] == 0x04`.
- **Big-Endian (Network Order, SPARC)**: Most Significant Byte stored first $\implies$ `bytes[0] == 0x01`.

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Unchecked Union Access (Type Confusion)**:
   Writing a `double` and reading a pointer member without verifying the type tag can produce invalid pointer addresses, leading to immediate segmentation faults or security exploits.
2. **Size Assumptions with Nested Structs**:
   A union containing a struct member will adopt that struct's alignment and size requirements.
3. **Strict Aliasing Violations Outside Unions**:
   Using `*(int*)&my_float` instead of a union is Undefined Behavior under strict aliasing rules (`-O2` optimization can reorder instructions dangerously).

---

## 7. Complexity & Production Best Practices
- **Tagged Union Wrappers**: Always pair unions with an `enum` discriminator tag inside an outer struct.
- **Hardware Register Mapping**: In embedded systems, use unions of bitfields and integer words to allow both atomic 32-bit register writes and individual bit-level control.

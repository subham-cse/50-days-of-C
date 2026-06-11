# Day 05: Data Types, Modifiers, Qualifiers, and Fixed-Width Architectures

## 1. Executive Overview & Mental Model
In C, a **Data Type** defines the memory allocation size, binary encoding representation, bitwise interpretation, and valid operations applicable to a variable. Because C allows direct hardware manipulation, understanding the underlying binary layout—from Two's Complement signed integers to IEEE 754 floating-point representations—is essential for writing bug-free, portable systems software.

```
+------------------------------------------------------------------------+
| Real-World Analogy: Physical Shipping Containers & Cargo Standards     |
| - A 20ft container (`int32_t`) has strict weight & volume limits.      |
| - If you pack standard dry cargo (signed int) vs refrigerated liquids  |
|   (float), the internal handling physics change completely, even if    |
|   the outer container occupies the exact same 32 bits of ship deck.    |
| - Ignoring weight limits leads to catastrophic hull failure (overflow).|
+------------------------------------------------------------------------+
```

---

## 2. Type Hierarchy & Architecture Data Models

```
                                  +-------------------+
                                  |    C Data Types   |
                                  +-------------------+
                                            |
        +------------------+----------------+------------------+------------------+
        |                  |                                   |                  |
        v                  v                                   v                  v
  [Basic Types]     [Derived Types]                     [User-Defined]         [Void]
   - char            - Pointers (`int *`)                - struct               - `void`
   - int             - Arrays (`int[10]`)                - union                - `void *`
   - float/double    - Functions (`int (*)(void)`)       - enum
   - _Bool (bool)
```

### Platform Data Models (Data Model Ambiguity)
The C standard specifies *minimum* bit widths, not exact sizes. Standard sizing varies across architectures:

| Type | ILP32 (32-bit Linux/Win) | LP64 (64-bit Linux/macOS) | LLP64 (64-bit Windows x64) |
| :--- | :--- | :--- | :--- |
| `char` | 8 bits (1 byte) | 8 bits (1 byte) | 8 bits (1 byte) |
| `short` | 16 bits (2 bytes) | 16 bits (2 bytes) | 16 bits (2 bytes) |
| `int` | 32 bits (4 bytes) | 32 bits (4 bytes) | 32 bits (4 bytes) |
| `long` | 32 bits (4 bytes) | **64 bits (8 bytes)** | **32 bits (4 bytes)** |
| `long long`| 64 bits (8 bytes) | 64 bits (8 bytes) | 64 bits (8 bytes) |
| Pointer (`void *`)| 32 bits (4 bytes) | **64 bits (8 bytes)** | **64 bits (8 bytes)** |

> [!IMPORTANT]
> Because `sizeof(long)` is 8 bytes on Linux x86_64 but 4 bytes on Windows x64, never use `long` for binary serialization or network protocols. Use fixed-width types from `<stdint.h>`.

---

## 3. Two's Complement Signed Integer Representation
Modern processors encode signed integers using **Two's Complement** arithmetic:
- Positive numbers: standard binary representation with Most Significant Bit (MSB) = `0`.
- Negative numbers: invert all bits (`~X`) and add `1` (`~X + 1`).

```
8-bit Signed Integer (int8_t) Examples:
  +5  =  0000 0101
 -5   =  1111 1010 + 1 = 1111 1011 (0xFB)
+127  =  0111 1111 (INT8_MAX)
-128  =  1000 0000 (INT8_MIN: asymmetrical because 0 is non-negative)
```

---

## 4. IEEE 754 Floating-Point Memory Layout

### Single-Precision (32-bit `float`):
```
 1 bit       8 bits                    23 bits
+------+----------------+-----------------------------------------------+
| Sign | Exponent (E)   | Fraction / Mantissa (M)                       |
|  [S] | [E7 . . . . E0]| [M22 . . . . . . . . . . . . . . . . . . . M0]|
+------+----------------+-----------------------------------------------+
 Bit 31   Bits 30-23       Bits 22-0
 Value = (-1)^S * (1.M) * 2^(E - 127)
```

### Double-Precision (64-bit `double`):
```
 1 bit       11 bits                   52 bits
+------+-------------------+--------------------------------------------+
| Sign | Exponent (E)      | Fraction / Mantissa (M)                    |
|  [S] | [E10 . . . . . E0]| [M51 . . . . . . . . . . . . . . . . . . M0]|
+------+-------------------+--------------------------------------------+
 Bit 63   Bits 62-52          Bits 51-0
 Value = (-1)^S * (1.M) * 2^(E - 1023)
```

---

## 5. Exact-Width `<stdint.h>` and `<stddef.h>` Types

```c
#include <stdint.h>
#include <stddef.h>

int8_t   / uint8_t   : Exactly 8-bit signed / unsigned integer
int16_t  / uint16_t  : Exactly 16-bit signed / unsigned integer
int32_t  / uint32_t  : Exactly 32-bit signed / unsigned integer
int64_t  / uint64_t  : Exactly 64-bit signed / unsigned integer
intptr_t / uintptr_t : Integer type guaranteed large enough to hold any pointer
size_t               : Unsigned integer for object sizes (result of sizeof)
ptrdiff_t            : Signed integer for pointer arithmetic subtraction
```

---

## 6. Type Promotion & Usual Arithmetic Conversions
When evaluating binary expressions with mixed types:
1. **Integer Promotion:** Types smaller than `int` (`char`, `short`, `bool`) are automatically promoted to `int` (or `unsigned int`).
2. **Usual Arithmetic Conversions (Hierarchy Rank):**
   $$\text{long double} > \text{double} > \text{float} > \text{unsigned long long} > \text{long long} > \text{unsigned long} > \text{long} > \text{unsigned int} > \text{int}$$

```
Expression:  int32_t a = -5;  uint32_t b = 10;
Evaluation:  if (a < b)  // DANGER!
Step 1: 'a' (-5) is converted to unsigned int.
Step 2: -5 in 32-bit unsigned arithmetic is 4,294,967,291.
Step 3: 4294967291 < 10 evaluates to FALSE (0)!
```

---

## 7. Common Pitfalls & Undefined Behaviors

| Hazard | Code Pattern | Consequence | Safe Pattern |
| :--- | :--- | :--- | :--- |
| **Signed Integer Overflow** | `int x = INT_MAX + 1;` | **Undefined Behavior (UB)**. Compiler may optimize out bounds checks. | Use unsigned arithmetic or check bounds prior to addition. |
| **Unsigned Wraparound Trap** | `for (unsigned i = 10; i >= 0; --i)` | Infinite loop. When `i` is 0, `0 - 1` wraps to `UINT_MAX`. | `for (size_t i = 10; i-- > 0; )` |
| **Signed vs Unsigned Comparison** | `if (-1 < sizeof(int))` | Evaluates to `false` because `sizeof` returns unsigned `size_t`. | `if ((ssize_t)-1 < (ssize_t)sizeof(int))` |
| **Float Equality Comparison** | `if (0.1 + 0.2 == 0.3)` | Evaluates to `false` due to binary IEEE 754 representation limits. | `if (fabs((a + b) - c) < 1e-9)` |

---

## 8. Best Practices Checklist
1. Default to `<stdint.h>` types (`uint32_t`, `int64_t`) for all embedded, network, and data-structure code.
2. Use `size_t` for all array indexing and memory buffer lengths.
3. Mark pointer arguments with `const` whenever data is only read.
4. Always compile with `-Wsign-compare` and `-Wconversion` to catch implicit truncation bugs.

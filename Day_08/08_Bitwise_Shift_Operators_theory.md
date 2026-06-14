# Day 08: Bitwise Operators, Shift Mechanics, and Low-Level Bit Manipulation

## 1. Executive Overview & Mental Model
Bitwise operators perform direct, deterministic, bit-level manipulations on the binary representations of integer operands within CPU registers. In systems programming, embedded firmware development, operating system kernels, and cryptography, bitwise operations form the bedrock of hardware peripheral configuration, memory-mapped I/O (MMIO) register control, network packet header serialization, and high-performance algorithms.

```
+------------------------------------------------------------------------+
| Real-World Analogy: Physical DIP Switch Panels                         |
| A 32-bit register is a bank of 32 physical switches on a circuit board.|
| Rather than replacing the whole board (overwriting all 32 bits),       |
| bitwise masks allow you to flip switch #4 ON without disturbing the     |
| cooling fans, power rails, or status LEDs on the other 31 switches.    |
+------------------------------------------------------------------------+
```

---

## 2. Bitwise Operators Taxonomy

```
Operator | Name                | Truth Table / Operation Formula
---------+---------------------+---------------------------------------------
&        | Bitwise AND         | 1 & 1 = 1; otherwise 0 (Intersection)
|        | Bitwise OR          | 0 | 0 = 0; otherwise 1 (Union)
^        | Bitwise XOR         | 1 ^ 0 = 1, 0 ^ 1 = 1; 1 ^ 1 = 0 (Difference)
~        | Bitwise NOT (Unary) | ~1 = 0, ~0 = 1 (One's Complement Inversion)
<<       | Left Shift          | x << n == x * 2^n (Zero-filled from right)
>>       | Right Shift         | x >> n (Logical: 0-fill, Arithmetic: sign-fill)
```

---

## 3. Fundamental Bit Manipulation Idioms

```
+-----------------------------------------------------------------------------+
| Operation             | Idiomatic C Expression                              |
+-----------------------+-----------------------------------------------------+
| Set bit 'n'           | `flags |= (1U << n);`                               |
| Clear bit 'n'         | `flags &= ~(1U << n);`                              |
| Toggle bit 'n'        | `flags ^= (1U << n);`                               |
| Test / Query bit 'n'  | `bool is_set = (flags >> n) & 1U;`                  |
| Clear Lowest Set Bit  | `val = val & (val - 1);`  (Kernighan's Algorithm)   |
| Isolate Lowest Set Bit| `lowest = val & (-val);`                            |
| Power of 2 Test       | `bool is_pow2 = (x != 0) && ((x & (x - 1)) == 0);`  |
+-----------------------------------------------------------------------------+
```

### Visual Diagram: Setting & Clearing Bit 3 in an 8-bit Register
```
Initial Value (0x24) :  0 0 1 0 0 1 0 0
Mask (1U << 3)       :  0 0 0 0 1 0 0 0
-----------------------------------------
1. SET BIT (OR | )   :  0 0 1 0 1 1 0 0  --> 0x2C (Bit 3 is now 1)

Inverted Mask (~Mask):  1 1 1 1 0 1 1 1
-----------------------------------------
2. CLEAR BIT (AND & ):  0 0 1 0 0 1 0 0  --> 0x24 (Bit 3 restored to 0)
```

---

## 4. Shift Mechanics: Logical vs. Arithmetic Right Shift

```
Logical Right Shift (Unsigned Types: uint32_t):
  Initial (0x80000004) : 1000 0000 0000 0000 0000 0000 0000 0100
  Shift `>> 2`         : 0010 0000 0000 0000 0000 0000 0000 0001 (Zeroes injected at MSB)

Arithmetic Right Shift (Signed Types: int32_t with negative value):
  Initial (-12)        : 1111 1111 1111 1111 1111 1111 1111 0100
  Shift `>> 2`         : 1111 1111 1111 1111 1111 1111 1111 1101 (-3: Sign bit replicated)
```

> [!CAUTION]
> Right-shifting a negative signed integer is **implementation-defined** in C99/C11. Left-shifting a negative signed integer is **Undefined Behavior (UB)**. Always use unsigned integer types (`uint32_t`, `uint64_t`) for bitwise shifts.

---

## 5. Under the Hood: CPU Instructions & Hardware Registers

```
C Expression               x86_64 Assembly Mnemonic   ARM64 Instruction
------------------------------------------------------------------------
x & y                      andl %esi, %edi            AND W0, W0, W1
x | y                      orl  %esi, %edi            ORR W0, W0, W1
x ^ y                      xorl %esi, %edi            EOR W0, W0, W1
~x                         notl %edi                  MVN W0, W0
x << n                     shll %cl, %edi             LSL W0, W0, W1
x >> n (unsigned logical)  shrl %cl, %edi             LSR W0, W0, W1
x >> n (signed arithmetic) sarl %cl, %edi             ASR W0, W0, W1
__builtin_popcount(x)      popcnt %edi, %eax          CNT V0.8B, V0.8B
```

---

## 6. Common Pitfalls & Undefined Behaviors

| Hazard | Bad Code | Consequence | Safe Pattern |
| :--- | :--- | :--- | :--- |
| **Shift Count $\ge$ Bit Width** | `uint32_t x = 1U << 32;` | **Undefined Behavior (UB)**. X86 masks shift count modulo 32 (`1 << 0 == 1`). | Ensure shift count is strictly in `[0, width - 1]`. |
| **Bitwise vs Logical Confusion** | `if (a & b)` vs `if (a && b)` | Logical checks truthiness of both; bitwise checks shared bit overlap. | Use `&&` for boolean conditions, `&` for bit masks. |
| **Operator Precedence Trap** | `if (val & 1 == 0)` | `==` has higher precedence than `&`! Parses as `val & (1 == 0)`. | Explicit parentheses: `if ((val & 1) == 0)` |
| **XOR Swap Pointer Aliasing** | `*a ^= *b; *b ^= *a; *a ^= *b;` | If `a == b`, variable zeroes itself out! | Use temporary variable or verify `a != b`. |

---

## 7. Best Practices
1. Always use `U` / `ULL` suffixes on bit-shift literals: `1ULL << 40`, never `1 << 40` (which overflows 32-bit `int`).
2. Wrap all bitwise expressions in parentheses when combined with arithmetic or relational operators.
3. Use descriptive macro masks or `enum` flags rather than hardcoded magic numbers.

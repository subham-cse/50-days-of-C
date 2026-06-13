# Day 07: Arithmetic, Relational, and Logical Operators

## 1. Executive Overview & Mental Model
Operators in C are fundamental syntactic tokens that command the CPU's Arithmetic Logic Unit (ALU) to execute computational transformations, relational assertions, or boolean truth-table reductions upon operands. Understanding operator semantics—including value category requirements (lvalue vs rvalue), short-circuit branch elimination, and sequence point side effects—is paramount to avoiding compiler undefined behavior.

```
+------------------------------------------------------------------------+
| Real-World Analogy: Electrical Logic Gates & Hydraulic Valves          |
| - Arithmetic operators are hydraulic pumps measuring and shifting flow.|
| - Relational operators are pressure sensors (yielding binary 0 or 1).  |
| - Logical operators (`&&`, `||`) are safety cutoff valves: if valve A  |
|   is closed, current never reaches valve B (Short-Circuit Evaluation). |
+------------------------------------------------------------------------+
```

---

## 2. Comprehensive Operator Classification

```
                          +------------------------------------+
                          |       Core Operator Families       |
                          +------------------------------------+
                                             |
            +--------------------------------+--------------------------------+
            |                                |                                |
            v                                v                                v
  [Arithmetic Operators]           [Relational Operators]           [Logical Operators]
   - Binary: `+`, `-`, `*`, `/`, `%` - Comparison: `<`, `<=`, `>`, `>=` - Binary: `&&` (AND), `||` (OR)
   - Unary:  `+`, `-`              - Equality:   `==`, `!=`         - Unary:  `!`  (Logical NOT)
   - Inc/Dec: `++`, `--`
```

---

## 3. Deep Dive into Operator Semantics

### A. Arithmetic Operators & Modulo Semantics (ISO C99 §6.5.5)
- **Integer Division (`/`):** Truncates any fractional component toward zero (Algebraic Truncation). Example: `7 / 2 == 3`, `-7 / 2 == -3`.
- **Modulo Operator (`%`):** Requires integer operands; forbidden on floating-point types (use `fmod()` from `<math.h>`).
- **Mathematical Invariant:**
  $$(a / b) \times b + (a \% b) == a$$
- **Sign Rule:** The result of `%` has the same sign as the dividend ($a$).
  - `7 % 3 == 1`
  - `-7 % 3 == -1`
  - `7 % -3 == 1`
  - `-7 % -3 == -1`

### B. Increment and Decrement: Prefix vs Postfix

```
Expression | Action & Return Value                         | Assembly Equivalent (x86_64)
-----------+-----------------------------------------------+------------------------------
++x        | Increments x, returns NEW incremented value.  | addl $1, (%rdi); movl (%rdi), %eax
x++        | Increments x, returns OLD original value.     | movl (%rdi), %eax; addl $1, (%rdi)
```

> [!WARNING]
> In C, `++x` and `x++` both produce **rvalues** (unlike C++ where `++x` produces an lvalue). You cannot write `++++x` in standard C.

### C. Logical Operators & Short-Circuit Evaluation
ISO C guarantees left-to-right evaluation with a **sequence point** immediately following the first operand:
1. `expr1 && expr2` : If `expr1` evaluates to `0` (false), `expr2` is **strictly not evaluated**.
2. `expr1 || expr2` : If `expr1` evaluates to non-zero (true), `expr2` is **strictly not evaluated**.

```
Safe Null Pointer Dereference Idiom:
  if (ptr != NULL && ptr->sensor_val > 50) {
      // Guaranteed safe: if ptr is NULL, ptr->sensor_val is NEVER accessed!
  }
```

---

## 4. Value Categories: Lvalues vs Rvalues

```
+-------------------------------------------------------------------------+
| Lvalue (Locator Value)                                                  |
| - Designates an object occupying an identifiable memory address.        |
| - Can appear on the left or right of an assignment operator `=`.        |
| - Examples: `variable`, `*ptr`, `arr[i]`, `struct_inst.field`          |
+-------------------------------------------------------------------------+
| Rvalue (Read/Value of an expression)                                    |
| - Transient data value resulting from expression computation.           |
| - Has no persistent memory address; cannot appear on left of `=`.      |
| - Examples: `42`, `x + 5`, `&variable`, `calculate_sum(a, b)`          |
+-------------------------------------------------------------------------+
```

---

## 5. Under the Hood: ALU Flags and Branch Prediction

```
C Comparison: `if (a < b)`
-------------------------------------------
Emitted Assembly (x86_64):
  movl -4(%rbp), %eax    # Load 'a' into EAX register
  cmpl -8(%rbp), %eax    # Compare 'b' with EAX (executes EAX - b)
  jge  .L_ELSE_BLOCK     # Jump if Greater or Equal (checks ALU flags)
```

### CPU Flag Evaluation:
- **Zero Flag (ZF):** Set to 1 if `a - b == 0` (used for `==` and `!=`).
- **Sign Flag (SF):** Set to 1 if the subtraction yielded a negative result.
- **Overflow Flag (OF):** Set to 1 if signed two's complement overflow occurred.
- Signed `<` condition is evaluated as: $\text{SF} \neq \text{OF}$.

---

## 6. Common Pitfalls & Undefined Behaviors

| Hazard | Buggy Code | Consequence | Safe Pattern |
| :--- | :--- | :--- | :--- |
| **Assignment in Conditional** | `if (x = 5)` | Assigns `5` to `x`, branch always executes. | `if (x == 5)` (or `if (5 == x)` Yoda syntax) |
| **Unsequenced Modifications**| `i = i++ + 1;` | **Undefined Behavior (UB)**: modifies `i` twice without sequence point. | `i = i + 2;` |
| **Division by Zero** | `int z = x / 0;` | Fatal hardware exception / SIGFPE signal crash. | `if (y != 0) z = x / y;` |
| **Side Effects in Short-Circuit**| `if (a || ++b)` | `b` is not incremented when `a` is true, causing logic desync. | Perform increment outside condition. |

---

## 7. Best Practices & Production Guidelines
1. Always wrap conditional assignments with explicit parentheses if intended: `if ((ptr = malloc(sz)) != NULL)`.
2. Do not write expressions that rely on operator evaluation order when not guaranteed by standard sequence points.
3. Leverage short-circuiting for defensive guard checks (bounds verification before array dereference).

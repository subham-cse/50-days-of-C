# Day 13: Switch-Case Control Structures and Jump Tables

## 1. Title & Overview
The **`switch-case`** statement is a multi-way branch selection construct in C optimized for evaluating a single discrete integral or enumeration expression against a fixed list of compile-time constants.

### Real-World Analogy
Think of an old rotary telephone or elevator floor selector:
- Pressing button `5` directly connects the circuit to Floor 5.
- The elevator controller does not test "Is it floor 1? No. Is it floor 2? No...". Instead, it directly uses the floor index to activate the target motor position via a direct lookup dispatch.

---

## 2. Deep Technical Dive & Memory Architecture

A `switch` expression must evaluate to an integral type (`char`, `short`, `int`, `long`, or `enum`). It cannot evaluate floating-point values or strings directly.

### Jump Table Resolution Architecture

When case values are densely populated within a compact numerical range, modern C compilers construct a **Jump Table** (an array of code address pointers).

```
                      +-----------------------------+
                      | switch (selector_val)       |
                      +--------------+--------------+
                                     |
                          [ Bounds Check: 0 <= val <= Max ]
                                     |
                       +-------------+-------------+
                       |                           |
                  (In Bounds)                 (Out of Bounds)
                       |                           |
                       v                           v
              +------------------+         +---------------+
              | Read Jump Table: |         | default: /    |
              | *jump_table[val] |         | exit switch   |
              +--------+---------+         +---------------+
                       |
        +--------------+--------------+--------------+
        |                             |              |
        v                             v              v
+---------------+             +---------------+ +---------------+
| Code: Case 0  |             | Code: Case 1  | | Code: Case N  |
+-------+-------+             +-------+-------+ +-------+-------+
        | (break)                     | (break)         | (break)
        +-----------------------------+-----------------+
                                      |
                                      v
                               [ Merge Point ]
```

---

## 3. Formal Syntax & Rules

```c
switch (integral_expression) {
    case CONSTANT_EXPRESSION_1:
        /* Statements */
        break; /* Terminates switch statement */

    case CONSTANT_EXPRESSION_2:
    case CONSTANT_EXPRESSION_3: /* Intentional grouping / shared handler */
        /* Statements */
        break;

    default:
        /* Fallback executed when no case constant matches */
        break;
}
```

### Key Language Rules:
1. **Case Labels:** Must be compile-time integer constant expressions.
2. **Duplicate Labels:** Multiple `case` labels with identical constant values within the same switch block produce a compile error.
3. **Fallthrough Semantics:** Omitting a `break` statement causes execution to fall through into subsequent case statements indiscriminately.

---

## 4. Comparison: Jump Tables vs Binary Decision Trees

| Property | Jump Table Optimization | Binary Decision Tree (Binary Search) | Linear Comparison Ladder |
| :--- | :--- | :--- | :--- |
| **Case Density** | High density (e.g., 0, 1, 2, 3, 4, 5) | Sparse / large gaps (e.g., 10, 1000, 500000) | Few branches (1-3 cases) |
| **Time Complexity** | $\mathcal{O}(1)$ Constant time dispatch | $\mathcal{O}(\log N)$ Binary search tree | $\mathcal{O}(N)$ Sequential testing |
| **Memory Overhead**| Table of $N$ code pointers in `.rodata` | Zero pointer table; instruction cache only | Zero pointer table |
| **Compiler Trigger**| Heuristic: density $> 40-50\%$ | Sparsely populated cases | Minimal number of cases |

---

## 5. Under the Hood: Assembly Translation & Duff's Device

### x86-64 Jump Table Assembly

```nasm
    movl    %edi, %eax                ; Load switch argument
    cmpl    $3, %eax                  ; Compare against max case index
    ja      .L_DEFAULT                ; If above unsigned max, jump to default
    jmp     *.L_JUMP_TABLE(,%rax,8)   ; Indirect jump: PC = table_base + (eax * 8)

.section .rodata
.L_JUMP_TABLE:
    .quad   .L_CASE_0
    .quad   .L_CASE_1
    .quad   .L_CASE_2
    .quad   .L_CASE_3
```

### Duff's Device: Loop Unrolling via Switch Fallthrough
In 1983, Tom Duff exploited C's switch-case fallthrough semantics to interleave loop unrolling with partial remainder iterations:

```c
void send(register short *to, register short *from, register int count) {
    register int n = (count + 7) / 8;
    switch (count % 8) {
        case 0: do { *to = *from++;
        case 7:      *to = *from++;
        case 6:      *to = *from++;
        case 5:      *to = *from++;
        case 4:      *to = *from++;
        case 3:      *to = *from++;
        case 2:      *to = *from++;
        case 1:      *to = *from++;
                } while (--n > 0);
    }
}
```

---

## 6. Common Pitfalls & Anti-Patterns

### 1. Unintentional Fallthrough Bugs
```c
switch (state) {
    case STATE_INIT:
        initialize_hardware();
        /* BUG: Forgot break! Falls through into STATE_RUN! */
    case STATE_RUN:
        start_worker_threads();
        break;
}
```
**Fix:** In modern C (C23 / GCC extensions), use `__attribute__((fallthrough));` or explicit comments `/* FALLTHROUGH */` to declare intentional fallthrough.

### 2. Variable Declarations Across Case Labels
C grammar prohibits jumping past a variable declaration with an initializer:
```c
switch (action) {
    case 1:
        int x = 42; /* COMPILE ERROR in C99 if used below without block scope */
        process(x);
        break;
    case 2:
        /* x is visible in scope, but initializer was skipped! */
        break;
}
```
**Fix:** Enclose case bodies in explicit compound braces `{ int x = 42; process(x); break; }`.

---

## 7. Production Best Practices

- Always provide a `default:` case, even if only to emit an assertion or error log (`assert(!"Unhandled switch case");`).
- Prefer `enum` types as switch selectors—compilers will issue `-Wswitch` warnings if any enumeration member is missing from the cases.

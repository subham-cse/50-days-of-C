# Day 14: The While Loop and Indeterminate Iteration

## 1. Title & Overview
The **`while` loop** is C's foundational entry-controlled loop construct. It repeatedly executes a target code block as long as its controlling predicate evaluates to true (non-zero). Because the condition is tested *before* entering the body on every iteration (including the very first), the loop executes zero or more times.

### Real-World Analogy
Consider pumping water out of a flooded basement:
- Check sensor: "Is water level $> 0$?"
- If *No* on the first check $\rightarrow$ pump never turns on.
- If *Yes* $\rightarrow$ run pump for one cycle, then re-check water level.
- Repeat until the sensor reports 0.

---

## 2. Deep Technical Dive & Control Flow

### ASCII Loop State Machine

```
                      +------------------------+
                      |      Entry Point       |
                      +-----------+------------+
                                  |
                                  v
                      +------------------------+ <----------+
                      |   Evaluate Condition   |            |
                      |      (expr != 0)       |            |
                      +-----------+------------+            |
                                  |                         |
                        +---------+---------+               |
                        |                   |               |
                 [ True / Non-zero ]    [ False / 0 ]       |
                        |                   |               |
                        v                   v               |
             +--------------------+   +-------------------+ |
             | Execute Loop Body  |   | Exit Loop Branch  | |
             +----------+---------+   +---------+---------+ |
                        |                       |           |
                        | Update State / Step   |           |
                        +-----------------------+-----------+
                                                |
                                                v
                                        [ Resume Program ]
```

---

## 3. Syntax & Grammar

### Formal Structure

```c
while (controlling_expression) {
    /* Loop Body: Executed zero or more times */
    /* Must eventually alter state involved in controlling_expression */
}
```

### Invariants & Semantics
- **Entry-Controlled:** If `controlling_expression` is false initially, the loop body is skipped entirely.
- **Side-Effects in Predicate:** `while (*dest++ = *src++);` is the canonical C idiom for string copying (`strcpy`), executing assignments and test simultaneously.

---

## 4. Comparison: `while` vs `for` vs `do-while`

| Feature | `while` Loop | `for` Loop | `do-while` Loop |
| :--- | :--- | :--- | :--- |
| **Control Type** | Entry-controlled | Entry-controlled | Exit-controlled |
| **Minimum Iterations** | 0 | 0 | 1 |
| **Iteration Domain** | Indeterminate / event-driven | Definite / counted intervals | Post-test validation / interactive |
| **Variable Scope** | Initializer scoped outside loop | Loop variable scoped to block | Initializer scoped outside loop |
| **Common Use Case** | Stream parsing, linked list traversal | Array indexing, fixed iteration count | Menu prompts, retry logic |

---

## 5. Under the Hood: Assembly & Loop Inversion Optimization

### Standard Assembly Layout
Compilers typically translate `while` loops by placing an unconditional jump to the bottom test (`loop inversion` / `test-at-bottom` pattern), saving an extra jump per iteration:

```nasm
    jmp     .L_TEST           ; Jump down to test first
.L_BODY:
    ; ... Loop body instructions ...
    addl    $1, -4(%rbp)      ; Increment state
.L_TEST:
    cmpl    $10, -4(%rbp)     ; Test condition
    jl      .L_BODY           ; If true, jump back up to body
```

### Indeterminate Termination & Invariants
Every correct `while` loop possesses a **Loop Variant** (a monotonic function that decreases towards a bounded limit) and a **Loop Invariant** (a logical assertion that remains true before and after every iteration). If a loop variant does not strictly progress towards termination, an **infinite loop** results.

---

## 6. Common Pitfalls & Edge Cases

### 1. Accidental Semicolon After Condition
```c
int count = 0;
while (count < 10); /* FATAL BUG: Null statement loop; loops infinitely! */
{
    count++;
}
```

### 2. Off-by-One in Pointer Traversal
```c
char *ptr = buffer;
while (*ptr != '\0') {
    /* If buffer is not null-terminated, this scans unmapped memory -> SEGFAULT */
    ptr++;
}
```
**Fix:** Always bound pointer increments with buffer length limits or ensure sentinel termination.

### 3. Floating-Point Loop Counters
```c
double x = 0.0;
while (x != 1.0) { /* BUG: Due to floating-point rounding, x may never EXACTLY equal 1.0 */
    x += 0.1;
}
```
**Fix:** Use `x < 1.0 - EPSILON` or integer loop counters.

---

## 7. Production Best Practices & Safety

- **Safety Caps / Watchdogs:** In embedded or real-time networking code, always implement a maximum iteration safeguard to prevent hardware lockup:
  ```c
  uint32_t timeout = 100000;
  while (!hardware_ready() && --timeout > 0);
  if (timeout == 0) handle_hardware_fault();
  ```
- **State Mutation:** Keep state mutation close to the loop boundary for maximum visibility and auditability.

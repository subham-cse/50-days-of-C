# Day 29: Recursion Fundamentals and Factorial Computation in C

## 1. Overview and Fundamental Principle
**Recursion** is an algorithmic technique where a function solves a problem by calling a smaller, self-similar instance of itself. 

Every mathematically valid recursive algorithm must satisfy two foundational rules:
1. **Base Case (Termination Condition):** A non-recursive branch that terminates the chain of recursive calls.
2. **Recursive Step (Progress Towards Base):** A step that reduces the problem size and moves strictly closer to the base case.

Without a robust base case, a function invokes itself endlessly until it exhausts the thread stack memory, triggering a **Stack Overflow** crash.

### Mathematical Formulation of Factorial
The factorial function $n!$ is defined over non-negative integers as:
$$n! = \begin{cases} 1 & \text{if } n = 0 \text{ or } n = 1 \quad \text{(Base Case)} \\ n \times (n - 1)! & \text{if } n > 1 \quad \text{(Recursive Step)} \end{cases}$$

---

## 2. Call Stack Unwinding & Stack Frames ("Under the Hood")

When computing `factorial(3)`, the execution involves two distinct phases: **Stack Expansion (Winding)** and **Stack Unwinding (Collapse)**.

```text
WINDING PHASE (Pushing Activation Records):
+------------------------------------+
| factorial(1) -> returns 1          |  <-- Base case hit! (Stack Depth = 3)
+------------------------------------+
| factorial(2) -> waits for fact(1)  |  (Stack Depth = 2)
+------------------------------------+
| factorial(3) -> waits for fact(2)  |  (Stack Depth = 1)
+------------------------------------+
| main()                             |  (Stack Base)
+------------------------------------+

UNWINDING PHASE (Resolving and Popping Frames):
1. factorial(1) returns 1; frame popped.
2. factorial(2) computes: 2 * 1 = 2; frame popped.
3. factorial(3) computes: 3 * 2 = 6; frame popped.
4. main() receives final value: 6.
```

---

## 3. Head Recursion vs Tail Recursion vs Iteration

### Tail Recursion & Compiler Optimization (TCO)
A recursive function is **tail-recursive** if the recursive call is the very last operation performed by the function (no pending multiplications or additions remain after the call returns).

```c
// Standard (Non-Tail) Recursion: Pending operation '* n'
uint64_t factorial_standard(uint64_t n) {
    if (n <= 1) return 1;
    return n * factorial_standard(n - 1); // Multiplication occurs AFTER return
}

// Tail Recursion: Accumulator parameter carries intermediate result
uint64_t factorial_tail_helper(uint64_t n, uint64_t accumulator) {
    if (n <= 1) return accumulator;
    return factorial_tail_helper(n - 1, n * accumulator); // Nothing left to do!
}
```

With compiler optimizations enabled (`-O2` or `-O3`), modern compilers perform **Tail Call Optimization (TCO)**, transforming tail-recursive calls into simple assembly `jmp` loops, reducing space complexity from $\mathcal{O}(N)$ stack frames to $\mathcal{O}(1)$.

---

## 4. Comparison Table: Recursive vs Iterative Factorial

| Metric | Standard Recursive | Tail Recursive (with TCO) | Iterative Loop (`for`) |
| :--- | :--- | :--- | :--- |
| **Time Complexity** | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ | $\mathcal{O}(N)$ |
| **Stack Space** | $\mathcal{O}(N)$ frames | $\mathcal{O}(1)$ frames (Optimized) | $\mathcal{O}(1)$ stack frame |
| **Stack Overflow Risk**| High for large $N$ | Low / None with TCO | Zero |
| **Readability** | Direct math translation | Requires accumulator | Highly practical |
| **Function Call Overhead** | Yes (`call`/`ret` per $N$) | Eliminated by compiler | Eliminated |

---

## 5. Integer Overflow Bounds for Factorial

Factorials grow extraordinarily fast (combinatorial explosion):
- $32$-bit `unsigned int`: Max $12! = 479,001,600$ ($13!$ overflows $2^{32} - 1$).
- $64$-bit `uint64_t`: Max $20! = 2,432,902,008,176,640,000$ ($21!$ overflows $2^{64} - 1$).

Calculating $n > 20$ in standard C without an arbitrary-precision big-integer library will silently produce wrapped/corrupted arithmetic values due to unsigned overflow.

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Missing Base Case:**
   Writing `return n * factorial(n - 1);` without `if (n <= 1) return 1;` runs infinitely until stack memory is exhausted (Segmentation fault).
2. **Negative Argument Inputs:**
   Calling `factorial(-5)` without an initial check `if (n < 0)` decrements `-5 -> -6 -> -7...`, never reaching `0` or `1`, causing infinite recursion.
3. **Stack Overflow via Deep Recursion:**
   On systems with limited stack sizes (e.g. embedded MCUs with 4 KB stacks), even moderate recursion depths can corrupt memory.

---

## 7. Production Best Practices

- Always validate inputs ($0 \le n \le 20$ for 64-bit unsigned integers).
- Provide safe iterative or tail-recursive implementations for performance-critical systems.
- Use explicit integer width types from `<stdint.h>` (`uint64_t`) to prevent platform-dependent integer truncation.

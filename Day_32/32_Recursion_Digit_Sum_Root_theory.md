# Day 32: Recursion — Digit Sum, Digit Count & Digital Root

## 1. Title & Overview
A **Digit Sum** $S(n)$ is the sum of all individual decimal digits of an integer $n$. 
A **Digital Root** (or repeated digital sum) $dr(n)$ is the single-digit value obtained by iteratively applying the digit sum function until a single-digit integer $[0, 9]$ remains.

### Real-World Analogy: Checksum Verification & Hash Buckets
Consider credit card numbers (Luhn checksum) or banking routing numbers. To verify data integrity over noisy channels, transmission protocols compress large multi-digit payloads into compact single-digit validation tags. Digital roots act as the simplest form of parity hashing—enabling quick divisibility verification and error detection via the arithmetic principle of **Casting Out Nines**.

---

## 2. Deep Technical Dive & Mathematical Proof

### 2.1 Recursive Formulation of Digit Sum
For any non-negative base-10 integer $n$:
$$S(n) = \begin{cases} 0 & \text{if } n = 0 \\ (n \pmod{10}) + S(\lfloor n / 10 \rfloor) & \text{if } n > 0 \end{cases}$$

For digit count:
$$C(n) = \begin{cases} 0 & \text{if } n = 0 \\ 1 + C(\lfloor n / 10 \rfloor) & \text{if } n > 0 \end{cases}$$
*(With special handling $C(0) = 1$ for the isolated literal zero).*

### 2.2 Digital Root & Modulo 9 Congruence Proof
Let $n$ be represented in decimal notation as:
$$n = \sum_{k=0}^{m} d_k \cdot 10^k \quad \text{where } d_k \in \{0, 1, \dots, 9\}$$

Notice that for all $k \ge 0$:
$$10 \equiv 1 \pmod 9 \implies 10^k \equiv 1^k \equiv 1 \pmod 9$$

Therefore:
$$n = \sum_{k=0}^m d_k \cdot 10^k \equiv \sum_{k=0}^m d_k \cdot 1 \equiv \sum_{k=0}^m d_k = S(n) \pmod 9$$

Because each iteration of the digit sum preserves the residue modulo 9, the terminal single digit $dr(n) \in \{1, \dots, 9\}$ satisfies:
$$dr(n) = \begin{cases} 0 & \text{if } n = 0 \\ 9 & \text{if } n \neq 0 \text{ and } n \equiv 0 \pmod 9 \\ n \pmod 9 & \text{otherwise} \end{cases}$$

Compact closed-form equation:
$$dr(n) = 1 + ((n - 1) \pmod 9) \quad \text{for } n > 0$$

---

## 3. ASCII Recursion Tree & Call Stack Visualization

### Recursive Digit Sum Call Stack for `sum_digits(9845)`
```text
[Call 1] sum_digits(9845) = (9845 % 10) + sum_digits(984) = 5 + [21] = 26
  |
  +--> [Call 2] sum_digits(984) = (984 % 10) + sum_digits(98) = 4 + [17] = 21
         |
         +--> [Call 3] sum_digits(98) = (98 % 10) + sum_digits(9) = 8 + [9] = 17
                |
                +--> [Call 4] sum_digits(9) = (9 % 10) + sum_digits(0) = 9 + [0] = 9
                       |
                       +--> [Call 5] sum_digits(0) ---> Base Case: returns 0
```

### Digital Root Reduction Flow
```text
           +-------------+
           |   n = 9845  |
           +------+------+
                  | sum_digits()
                  v
           +-------------+
           |   S = 26    |  (26 >= 10 -> recurse)
           +------+------+
                  | sum_digits()
                  v
           +-------------+
           |    S = 8    |  (8 < 10 -> Terminal Single Digit)
           +-------------+
   Closed Form Verification: 1 + ((9845 - 1) % 9) = 1 + (9844 % 9) = 1 + 7 = 8
```

---

## 4. Algorithmic Comparison

| Approach | Time Complexity | Auxiliary Space | Tail Recursion Possible? | Robustness to Large Input |
| :--- | :--- | :--- | :--- | :--- |
| **Standard Recursive** | $O(\log_{10} n)$ | $O(\log_{10} n)$ stack frames | No (addition pending) | Limited by stack depth |
| **Tail-Recursive Accumulator** | $O(\log_{10} n)$ | $O(1)$ with compiler optimization | Yes (`acc + n % 10`) | Excellent |
| **Iterative Loop** | $O(\log_{10} n)$ | $O(1)$ | N/A | Excellent |
| **$O(1)$ Formula Digital Root** | $O(1)$ | $O(1)$ | N/A | Instantaneous |

---

## 5. Under the Hood: Register Allocation & Modulo Division
- In x86-64 assembly, division by 10 (`n / 10` and `n % 10`) is often compiled via **multiplicative inverse tricks** using `imul` with a magic constant (`0xCCCCCCCD`) instead of the slow hardware instruction `idiv`.
- In standard recursion, the activation frame must store the local state `(n % 10)` across the recursive call to `sum_digits(n / 10)` before the final addition occurs, consuming stack memory.
- In accumulator-based tail recursion (`sum_digits_tail(n / 10, acc + n % 10)`), the intermediate value is stored directly in an CPU register (e.g., `%rsi`), allowing the compiler to rewrite the recursion as a simple loop (`jmp`).

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Negative Integer Inputs**:
   `-1234 % 10` in C99 yields `-4`, causing negative digit sums or infinite recursion if not normalized with absolute values.
2. **`LLONG_MIN` Negation Overflow**:
   Performing `-n` on `LLONG_MIN` overflows standard signed integer bounds.
3. **Infinite Recursion on Floating Point Conversion**:
   Attempting digit extraction on float representations leads to precision loss and non-terminating loops.

---

## 7. Complexity & Production Best Practices
- **Time Complexity**: $O(\log_{10} n)$ for digit sum; $O(1)$ for mathematical digital root.
- **Space Complexity**: $O(\log_{10} n)$ stack space (non-tail recursive); $O(1)$ auxiliary memory (iterative or math formula).
- **Production Tip**: Use the $O(1)$ formula `1 + (n - 1) % 9` for fast digital root checks in high-throughput validation pipelines, but use recursive digit accumulators when digit transformation (e.g. Luhn algorithm doubling) is required.

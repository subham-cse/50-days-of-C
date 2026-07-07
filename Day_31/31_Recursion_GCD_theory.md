# Day 31: Recursion — Greatest Common Divisor (Euclidean Algorithm) & Extended GCD

## 1. Title & Overview
The **Greatest Common Divisor (GCD)**—also known as the Greatest Common Factor (GCF) or Highest Common Factor (HCF)—of two non-zero integers $a$ and $b$ is the largest positive integer $d$ such that $d \mid a$ and $d \mid b$.

### Real-World Analogy: Tiling a Floor
Imagine you need to pave a rectangular courtyard of dimensions $A \times B$ meters using identical square tiles with the largest possible integer edge length without cutting any tile. 
- If $A = 252$ and $B = 105$, fitting square tiles of size $105 \times 105$ leaves an uncovered strip of $252 \pmod{105} = 42 \times 105$.
- The problem of tiling the remaining strip reduces to tiling a smaller rectangle of dimensions $105 \times 42$.
- Subdividing further yields $105 \pmod{42} = 21 \times 42$.
- Finally, $42 \pmod{21} = 0$, meaning tiles of dimension $21 \times 21$ pave the entire area perfectly. Thus, $\gcd(252, 105) = 21$.

---

## 2. Deep Technical Dive & Mathematical Proof

### 2.1 The Euclidean Principle
The Euclidean algorithm relies on the theorem:
$$\gcd(a, b) = \gcd(b, a \pmod b) \quad \text{for } b \neq 0$$
$$\gcd(a, 0) = |a|$$

**Proof:**
Let $a = q \cdot b + r$, where $q = \lfloor a / b \rfloor$ and $r = a \pmod b$.
1. Suppose $d \mid a$ and $d \mid b$. Then $a = d \cdot x$ and $b = d \cdot y$.
   $$r = a - q \cdot b = d \cdot x - q \cdot (d \cdot y) = d(x - q \cdot y)$$
   Hence, $d \mid r$. Thus any common divisor of $a$ and $b$ is also a common divisor of $b$ and $r$.
2. Conversely, suppose $c \mid b$ and $c \mid r$. Then $a = q \cdot b + r$ is a multiple of $c$.
   Therefore, the set of common divisors of $\{a, b\}$ is identical to the set of common divisors of $\{b, r\}$.
   Consequently, $\max(\text{Divisors}(a, b)) = \max(\text{Divisors}(b, r))$, completing the proof.

### 2.2 Extended Euclidean Algorithm & Bézout's Identity
Bézout's identity states that for non-zero integers $a$ and $b$, there exist integers $x$ and $y$ (Bézout coefficients) such that:
$$a \cdot x + b \cdot y = \gcd(a, b)$$

The Extended Euclidean algorithm computes not only $\gcd(a, b)$ but also $x$ and $y$ recursively:
- Base case: If $b = 0$, $a \cdot 1 + 0 \cdot 0 = a = \gcd(a, 0)$. Thus $x = 1, y = 0$.
- Recursive step: Let $x_1, y_1$ satisfy $b \cdot x_1 + (a \pmod b) \cdot y_1 = g$.
  Since $a \pmod b = a - \lfloor a / b \rfloor \cdot b$:
  $$b \cdot x_1 + \left(a - \left\lfloor \frac{a}{b} \right\rfloor \cdot b\right) \cdot y_1 = g$$
  $$a \cdot y_1 + b \cdot \left(x_1 - \left\lfloor \frac{a}{b} \right\rfloor \cdot y_1\right) = g$$
  Matching coefficients gives:
  $$x = y_1, \quad y = x_1 - \left\lfloor \frac{a}{b} \right\rfloor \cdot y_1$$

---

## 3. ASCII Call Tree & Stack Frame Flowchart

### Call Stack for `gcd(252, 105)`
```text
  [Call 1] gcd(252, 105) -----------------------------> Returns 21
     |
     v   (252 % 105 = 42)
  [Call 2] gcd(105, 42) ------------------------------> Returns 21
     |
     v   (105 % 42 = 21)
  [Call 3] gcd(42, 21) -------------------------------> Returns 21
     |
     v   (42 % 21 = 0)
  [Call 4] gcd(21, 0) ---> Base Case reached (b == 0) -> Returns 21
```

### Memory Stack Frame Allocation during `gcd_extended(30, 20, &x, &y)`
```text
+--------------------------------------------------------------+
| Frame 3: gcd_extended(10, 0, &x, &y)                         |
|   a = 10, b = 0 | *x = 1, *y = 0 | returns 10                |
+--------------------------------------------------------------+
| Frame 2: gcd_extended(20, 10, &x, &y)                        |
|   a = 20, b = 10 | x1 = 1, y1 = 0                            |
|   *x = 0, *y = 1 - (20/10)*0 = 1 | returns 10                |
+--------------------------------------------------------------+
| Frame 1: gcd_extended(30, 20, &x, &y)                        |
|   a = 30, b = 20 | x1 = 0, y1 = 1                            |
|   *x = 1, *y = 0 - (30/20)*1 = -1 | returns 10               |
+--------------------------------------------------------------+
Verification: 30*(1) + 20*(-1) = 30 - 20 = 10 == gcd(30, 20)
```

---

## 4. Algorithmic Comparison

| Approach | Time Complexity | Auxiliary Space | Tail Recursive? | Hardware Branch Efficiency |
| :--- | :--- | :--- | :--- | :--- |
| **Euclidean Recursive** | $O(\log(\min(a, b)))$ | $O(\log(\min(a, b)))$ stack | Yes (Tail call optimizable) | High (modulo division) |
| **Euclidean Iterative** | $O(\log(\min(a, b)))$ | $O(1)$ | N/A | High |
| **Extended Euclidean** | $O(\log(\min(a, b)))$ | $O(\log(\min(a, b)))$ stack | No (unwinding computation) | Moderate |
| **Binary GCD (Stein's)**| $O(\log^2(\max(a, b)))$ bitwise | $O(1)$ iterative | N/A | Very High (no division) |

---

## 5. Under the Hood: Lamé's Theorem & Assembly Tail-Call Optimization
1. **Lamé's Theorem (1844)**: The number of division steps in the Euclidean algorithm applied to two integers $a > b$ never exceeds 5 times the number of digits in $b$ (base 10). The worst-case input pairs are consecutive Fibonacci numbers $F_{k+1}$ and $F_k$ because consecutive Fibonacci quotients are always 1, resulting in the slowest convergence.
2. **Compiler Tail Call Optimization (TCO)**:
   In `gcd(int a, int b) { if (b == 0) return a; return gcd(b, a % b); }`, the recursive call is in strict tail position. When compiled with `-O2` or `-O3`, modern compilers (GCC/Clang) eliminate stack frame growth completely by transforming the recursive call into a `jmp` instruction with register updates (`mov` and `idiv`).

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Negative Inputs & C Modulo**:
   In C99, integer division truncates towards zero (`-7 / 3 == -2`, `-7 % 3 == -1`). If negative inputs are passed directly to Euclidean recursion without taking absolute values, the termination condition `b == 0` is reached, but the return value can be negative. GCD by definition is strictly positive: $\gcd(a, b) \ge 1$.
2. **`INT_MIN` Absolute Value Overflow**:
   Computing `abs(INT_MIN)` produces Undefined Behavior (UB) because $|-2^{31}| = 2^{31}$, which exceeds `INT_MAX` ($2^{31}-1$) in 2's complement 32-bit arithmetic.
3. **Division by Zero**:
   If $a = 0$ and $b = 0$, $\gcd(0, 0)$ is mathematically undefined (every integer divides 0). Implementing code must handle $(0, 0)$ gracefully.

---

## 7. Complexity & Production Best Practices
- **Time Complexity**: $O(\log(\min(a, b)))$ arithmetic steps.
- **Space Complexity**: $O(1)$ when iterative or with TCO enabled; $O(\log(\min(a, b)))$ recursive stack frames without TCO.
- **Production Advice**:
  - Always normalize inputs to `unsigned long long` or handle `INT_MIN` before calling the core recursive helper.
  - Extended GCD is the backbone of RSA public-key cryptography for finding modular multiplicative inverses where $a \cdot x \equiv 1 \pmod m \iff a \cdot x + m \cdot y = 1$.

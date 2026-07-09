# Day 33: Recursion — Fast Power & Modular Exponentiation (Binary Exponentiation)

## 1. Title & Overview
**Binary Exponentiation** (also known as *exponentiation by squaring*) is an algorithmic technique for computing integer powers $x^n$ in $O(\log n)$ multiplications rather than the naive $O(n)$ sequential multiplications.

### Real-World Analogy: Folding a Paper Strip
If you need a strip of paper $1024$ layers thick:
- **Naive approach**: Place single sheets one on top of another 1024 times (1024 operations).
- **Binary exponentiation**: Fold a single sheet in half 10 times ($2^{10} = 1024$). Each fold doubles the thickness instantaneously ($10$ operations).

In public-key cryptography (RSA, Diffie-Hellman, Elliptic Curve Cryptography), systems frequently raise 2048-bit numbers to 2048-bit powers modulo a 2048-bit prime. Naive multiplication would require $\approx 2^{2048}$ operations (more than the atoms in the observable universe). Binary exponentiation completes this in $\le 4096$ operations.

---

## 2. Deep Technical Dive & Mathematical Proof

### 2.1 Divide-and-Conquer Recurrence
For any base $x \in \mathbb{R}$ and integer exponent $n \ge 0$:
$$x^n = \begin{cases} 
1 & \text{if } n = 0 \\ 
(x^{n/2})^2 & \text{if } n \text{ is even} \\ 
x \cdot (x^{(n-1)/2})^2 & \text{if } n \text{ is odd} 
\end{cases}$$

For negative exponents $n < 0$:
$$x^n = \frac{1}{x^{-n}} = \left(\frac{1}{x}\right)^{-n}$$

### 2.2 Modular Exponentiation Properties
In cryptography, we compute $(a^b) \pmod m$. Using modular multiplication congruence:
$$(a \cdot b) \pmod m = ((a \pmod m) \cdot (b \pmod m)) \pmod m$$
Applying this at each squaring step prevents intermediate values from overflowing hardware registers:
$$\text{mod\_pow}(x, n, m) = \begin{cases}
1 \pmod m & \text{if } n = 0 \\
(\text{mod\_pow}(x, n/2, m)^2) \pmod m & \text{if } n \text{ is even} \\
(x \cdot (\text{mod\_pow}(x, n/2, m)^2 \pmod m)) \pmod m & \text{if } n \text{ is odd}
\end{cases}$$

---

## 3. ASCII Recursion Tree & Exponentiation Trace

### Recursive Call Tree for $x^{13}$ ($13_{10} = 1101_2$)
```text
                       pow(x, 13)
                           |   [odd: x * (pow(x, 6))^2]
                           v
                       pow(x, 6)
                           |   [even: (pow(x, 3))^2]
                           v
                       pow(x, 3)
                           |   [odd: x * (pow(x, 1))^2]
                           v
                       pow(x, 1)
                           |   [odd: x * (pow(x, 0))^2]
                           v
                       pow(x, 0) ---> Base Case: returns 1.0
```

### Unwinding and Multiplication Reconstruction
```text
Step 0: pow(x, 0) = 1
Step 1: pow(x, 1) = x * (1)^2           = x^1
Step 2: pow(x, 3) = x * (x^1)^2         = x^1 * x^2 = x^3
Step 3: pow(x, 6) = (x^3)^2             = x^6
Step 4: pow(x, 13) = x * (x^6)^2        = x * x^12 = x^13
Total multiplications: 5 (vs 13 in naive loop)
```

---

## 4. Algorithmic Comparison

| Approach | Time Complexity | Multiplications for $n=10^6$ | Space Complexity | Float & Negative Support |
| :--- | :--- | :--- | :--- | :--- |
| **Naive Recursive** | $O(n)$ | $1,000,000$ | $O(n)$ stack | Complex |
| **Binary Exp (Recursive)**| $O(\log n)$ | $\approx 20$ | $O(\log n)$ stack | Yes ($x \in \mathbb{R}$) |
| **Binary Exp (Iterative)**| $O(\log n)$ | $\approx 20$ | $O(1)$ | Yes |
| **Modular Exp (Recursive)**| $O(\log n)$ | $\approx 20$ | $O(\log n)$ stack | Integers only |

---

## 5. Under the Hood: Bit Representation & Register Overflow
- **Bitwise Relation**: The exponent $n$ can be scanned bit-by-bit:
  $$n = \sum_{i=0}^k b_i \cdot 2^i \implies x^n = \prod_{i=0}^k x^{b_i \cdot 2^i}$$
  Whenever bit $b_i = 1$, the accumulated result is multiplied by the current power-of-two square $x^{2^i}$.
- **Preventing 64-bit Multiplication Overflow**:
  In modular exponentiation with 64-bit moduli $m \approx 10^{18}$, $(a \cdot b)$ can reach $10^{36}$, overflowing `uint64_t`. In C99, 128-bit types (`unsigned __int128` in GCC/Clang) or modular multiplication functions are required to store intermediate products safely before modulo reduction.

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Recomputing Recursive Sub-problems Naively**:
   Writing `pow(x, n/2) * pow(x, n/2)` invokes two recursive branches, degenerating the complexity from $O(\log n)$ back to $O(n)$. Always store `half = pow(x, n/2)` into a local variable.
2. **Negative Exponent Integer Division**:
   Passing integer negative exponents to `int_pow(2, -3)` results in `0` due to integer truncation. Negative exponents require `double` returns.
3. **`0^0` Indeterminacy**:
   In standard algebra and IEEE 754 floating-point math, `pow(0.0, 0)` is defined as `1.0`. For base `0.0` with negative exponent, it represents division by zero (`+INFINITY`).

---

## 7. Complexity & Production Best Practices
- **Time Complexity**: $O(\log n)$ multiplications and divisions.
- **Space Complexity**: $O(\log n)$ recursion stack frames; $O(1)$ iterative.
- **Production Tip**: Always use modular exponentiation with 64-bit or 128-bit unsigned types for cryptography and competitive programming to eliminate integer overflow bugs.

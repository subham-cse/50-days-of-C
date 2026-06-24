# Day 18: C Standard Math Library: Powers, Exponentiation, and Roots

## 1. Title & Overview
The C standard library header `<math.h>` provides high-precision mathematical functions implementing floating-point arithmetic compliant with **IEEE 754**. Functions like `pow()`, `sqrt()`, `cbrt()`, and `hypot()` perform floating-point exponentiation and root extraction.

### Real-World Analogy
Consider calculating the direct Euclidean line-of-sight distance across 3D airspace for air traffic control, or computing compound interest:
- Distance: $d = \sqrt{\Delta x^2 + \Delta y^2 + \Delta z^2}$
- Investment growth: $A = P \cdot (1 + r/n)^{nt}$

These operations require hardware-accelerated floating-point calculation units (FPU) to maintain numerical stability and avoid catastrophic cancellation.

---

## 2. Deep Technical Dive & Memory Architecture

### IEEE 754 Double Precision (64-bit Binary Representation)

```
 +---+--------------------+---------------------------------------------------+
 | s |   Exponent (11)    |                  Mantissa (52)                    |
 +---+--------------------+---------------------------------------------------+
  63  62                52 51                                                0
```
- **Sign bit (bit 63):** `0` = positive, `1` = negative.
- **Biased Exponent (bits 62-52):** Bias of $1023$.
- **Mantissa/Fraction (bits 51-0):** 52 bits affording $\approx 15-17$ significant decimal digits of precision.

### Mathematical Domain and Error Handling
C99 math functions communicate errors via:
1. `errno` set to `EDOM` (Domain Error) or `ERANGE` (Range Error).
2. Floating-point exceptions (e.g., `FE_INVALID`, `FE_DIVBYZERO`, `FE_OVERFLOW` in `<fenv.h>`).
3. Special IEEE 754 values: `NaN` (Not a Number) and `+/-HUGE_VAL` ($\pm \infty$).

---

## 3. Standard Function Signatures & Types

| Function Prototype | Domain | Range | Special Handling |
| :--- | :--- | :--- | :--- |
| `double sqrt(double x)` | $x \ge 0$ | $[0, +\infty)$ | If $x < 0$, returns `NaN` and raises `FE_INVALID` / sets `EDOM` |
| `double cbrt(double x)` | $(-\infty, +\infty)$ | $(-\infty, +\infty)$ | Preserves sign: $\text{cbrt}(-8.0) = -2.0$ |
| `double pow(double base, double exp)` | Base/exp dependent | Real numbers | Domain error if base $< 0$ and exp is non-integer |
| `double hypot(double x, double y)` | $(-\infty, +\infty)$ | $[0, +\infty)$ | Avoids intermediate overflow/underflow in $\sqrt{x^2 + y^2}$ |

*(Note: Float variants end in `f` such as `sqrtf()`, `powf()`; long double variants end in `l` such as `sqrtl()`, `powl()`.)*

---

## 4. Comparison: `pow(x, 2.0)` vs Direct Multiplication `x * x`

| Metric | `x * x` (Direct Multiplication) | `pow(x, 2.0)` (`<math.h>`) |
| :--- | :--- | :--- |
| **Instruction Count** | 1 hardware cycle (`vmulsd` / `fmul`) | 30 to 80+ instructions (log-exp approximation call) |
| **Performance Overhead** | Sub-nanosecond | High latency function call |
| **Compiler Optimization** | Naturally vectorized | May be simplified under `-ffast-math` / `-O3` |
| **Precision** | Exact bitwise IEEE single multiplication | Subject to series approximation rounding |

---

## 5. Under the Hood: Square Root Hardware Instructions & The Fast Inverse Sqrt

### Hardware Instruction (`sqrtsd`)
On modern x86 CPUs with SSE2/AVX, `sqrt()` is mapped directly to hardware instructions:
```nasm
sqrtsd  %xmm0, %xmm0    ; Computes sqrt of double in single hardware step
```

### The Quake III "Fast Inverse Square Root" Hack
Before dedicated hardware instructions, John Carmack's team at id Software developed the infamous IEEE 754 bit-manipulation algorithm for $1/\sqrt{x}$:

```c
float Q_rsqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // Bit-level floating point cast
    i  = 0x5f3759df - ( i >> 1 );               // Magic constant approximation
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st Newton-Raphson iteration
    return y;
}
```

---

## 6. Common Pitfalls & Undefined Behaviors

### 1. Linking Error: Missing `-lm` Flag
On POSIX/GCC systems, standard math functions reside in `libm`. Failing to pass `-lm` produces linker errors:
```
undefined reference to `pow'
undefined reference to `sqrt'
```
**Fix:** Always compile math-dependent C files with `gcc -Wall file.c -lm -o file`.

### 2. Domain Errors on Negative Square Root
```c
double val = sqrt(-5.0); /* Returns NaN; sets errno = EDOM */
if (val != val) { /* True only for NaN! */
    printf("Result is NaN!\n");
}
```
**Fix:** Use `isnan(val)` from `<math.h>` to test for invalid results.

---

## 7. Production Best Practices

- Always validate inputs prior to `sqrt()` ($x \ge 0.0$) and `pow()` (avoid fractional exponents on negative bases).
- Prefer direct algebraic multiplication (`x * x * x`) over `pow(x, 3.0)` for small integer powers.
- Always link with `-lm` when compiling across Linux/POSIX environments.

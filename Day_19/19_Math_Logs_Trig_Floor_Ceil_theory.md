# Day 19: Advanced Math: Logarithms, Trigonometry, and Rounding Functions

## 1. Title & Overview
The standard C math library `<math.h>` extends beyond elementary algebra to provide core scientific computing primitives: **logarithmic and exponential functions** (`log`, `log10`, `log2`, `exp`), **trigonometric and inverse trigonometric functions** (`sin`, `cos`, `tan`, `atan2`), and **numerical discretization/rounding functions** (`floor`, `ceil`, `round`, `trunc`).

### Real-World Analogy
Consider game physics or GPS navigation:
- Determining the heading angle between two world coordinates requires the four-quadrant arctangent: $\theta = \text{atan2}(\Delta y, \Delta x)$.
- Calculating decibel sound attenuation or data entropy requires logarithms ($\log_{10}$, $\log_2$).
- Mapping continuous spatial physics coordinates into discrete tile-grid indices requires `floor()`.

---

## 2. Deep Technical Dive & Function Classifications

### 1. Logarithmic & Exponential Foundations
- `log(x)`: Natural logarithm $\ln(x)$ (base $e \approx 2.7182818$).
- `log10(x)`: Common logarithm (base 10). Useful for acoustic decibels, Richter scale, and calculating integer digit counts: $\lfloor \log_{10}(N) \rfloor + 1$.
- `log2(x)`: Binary logarithm (base 2). Fundamental for information theory and Shannon entropy ($H = -\sum p_i \log_2 p_i$).
- `exp(x)`: Computes $e^x$.

### 2. Trigonometry & The Four-Quadrant Arctangent
Standard C trigonometric functions accept arguments strictly in **radians**, not degrees ($\text{rad} = \text{deg} \times \frac{\pi}{180}$).

```
                             +y (90 deg / PI/2)
                                     |
             Quadrant II             |             Quadrant I
       (-x, +y) -> atan2 > 0         |       (+x, +y) -> atan2 > 0
                                     |
     -x (180 deg / PI) --------------+-------------- +x (0 deg / 0 rad)
                                     |
             Quadrant III            |             Quadrant IV
       (-x, -y) -> atan2 < 0         |       (+x, -y) -> atan2 < 0
                                     |
                            -y (270 deg / -PI/2)
```
- `atan2(y, x)` resolves the ambiguous quadrant dilemma of simple `atan(y/x)` and safely handles $x = 0$ without division-by-zero crashes.

### 3. Rounding & Discretization Mechanics
- **`floor(x)`:** Largest integral value not greater than $x$ ($\lfloor x \rfloor$, rounds towards $-\infty$).
- **`ceil(x)`:** Smallest integral value not less than $x$ ($\lceil x \rceil$, rounds towards $+\infty$).
- **`trunc(x)`:** Rounds towards zero, discarding fractional bits.
- **`round(x)`:** Rounds to nearest integer, rounding halfway cases away from zero.

---

## 3. Discretization Behavior Comparison Matrix

| Input $x$ | `floor(x)` | `ceil(x)` | `trunc(x)` | `round(x)` |
| :---: | :---: | :---: | :---: | :---: |
| **$+2.3$** | $+2.0$ | $+3.0$ | $+2.0$ | $+2.0$ |
| **$+2.7$** | $+2.0$ | $+3.0$ | $+2.0$ | $+3.0$ |
| **$+2.5$** | $+2.0$ | $+3.0$ | $+2.0$ | $+3.0$ |
| **$-2.3$** | $-3.0$ | $-2.0$ | $-2.0$ | $-2.0$ |
| **$-2.7$** | $-3.0$ | $-2.0$ | $-2.0$ | $-3.0$ |
| **$-2.5$** | $-3.0$ | $-2.0$ | $-2.0$ | $-3.0$ |

---

## 4. Under the Hood: CORDIC Algorithms & FPU Instructions

Trigonometric functions are evaluated using hardware instructions or polynomial series approximations:
1. **CORDIC (Coordinate Rotation Digital Computer):** Iterative shift-and-add algorithm implemented directly in microcode for calculating trigonometric and hyperbolic functions.
2. **Taylor / Chebyshev Polynomial Expansions:** Minimax polynomials optimized for floating-point intervals with bounded relative error ($< 0.5 \text{ ULP}$).

```nasm
; Hardware trigonometric instruction on x86 FPU
fldl    -8(%rbp)        ; Push x onto floating point stack
fsin                    ; Replace top of stack with sin(x)
fstpl   -16(%rbp)       ; Store result
```

---

## 5. Common Pitfalls & Numerical Instabilities

### 1. Degree vs Radian Conversion Oversight
```c
double angle_deg = 90.0;
double res = sin(angle_deg); /* BUG: sin(90.0 rad) != sin(90 deg)! */
```
**Fix:** Define `M_PI` (or `3.14159265358979323846`) and convert: `sin(angle_deg * (M_PI / 180.0))`.

### 2. Logarithm Domain Violations
Passing $x \le 0$ to any `log()` function produces a domain error:
```c
double l = log(0.0);  /* Returns -HUGE_VAL (-infinity), sets ERANGE */
double l2 = log(-1.0); /* Returns NaN, sets EDOM */
```

---

## 6. Production Best Practices

- Always use `atan2(y, x)` instead of `atan(y / x)` for direction and navigation.
- Ensure inputs to `acos()` and `asin()` are clamped to the valid interval $[-1.0, +1.0]$ to guard against floating-point inaccuracies pushing values slightly out of range (e.g., $1.0000000000000002$).

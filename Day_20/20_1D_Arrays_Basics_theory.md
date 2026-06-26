# Day 20: One-Dimensional Arrays: Memory Architecture and Indexing

## 1. Title & Overview
A **one-dimensional (1D) array** in C is a contiguous, fixed-size sequence of elements of homogeneous data type allocated in contiguous memory addresses. Arrays form the bedrock of memory manipulation in C and share a profound, intimate duality with pointers.

### Real-World Analogy
Consider a row of identical numbered storage lockers in a train station:
- Every locker has an identical width (e.g., `sizeof(int) = 4` bytes).
- The station master knows only the base memory address of locker #0.
- Finding locker #$i$ requires no sequential searching: simply calculate $\text{BaseAddress} + (i \times \text{LockerWidth})$.

---

## 2. Deep Technical Dive: Contiguous Memory Layout

In C, array indexing is zero-based ($0 \dots N-1$). The address of element $i$ is calculated directly by the compiler:

$$\text{Address}(\text{arr}[i]) = \text{BaseAddress} + i \times \text{sizeof}(\text{Type})$$

### Contiguous Byte Layout in Memory (e.g., `int arr[4]` at Base `0x1000`)

```
 Memory Address:  0x1000      0x1004      0x1008      0x100C      0x1010
                 +-----------+-----------+-----------+-----------+
 Byte Cells:     |  arr[0]   |  arr[1]   |  arr[2]   |  arr[3]   |
                 +-----------+-----------+-----------+-----------+
 Offset:         +0 bytes    +4 bytes    +8 bytes    +12 bytes
 Array Index:        [0]         [1]         [2]         [3]
 Pointer Equiv:    *(arr+0)    *(arr+1)    *(arr+2)    *(arr+3)
```

---

## 3. Formal Syntax & Initialization Grammar

```c
/* 1. Uninitialized Stack Array (Contains indeterminate garbage values) */
int raw_buffer[10];

/* 2. Full Aggregate Initialization */
int primes[5] = {2, 3, 5, 7, 11};

/* 3. Zero-Initialization Idiom */
int zero_filled[100] = {0}; /* First element 0, remaining implicitly 0 */

/* 4. Inferred Size Array */
int days_in_quarters[] = {90, 91, 92, 92}; /* Compiler determines size = 4 */

/* 5. C99 Designated Initializers */
int sparse[10] = {[0] = 1, [5] = 99, [9] = 1000};
```

---

## 4. Pointer-Array Duality & Array Decay

In most C expressions, the name of an array **decays** (automatically converts) into a pointer to its first element (`&arr[0]`):

$$\text{arr}[i] \iff *(\text{arr} + i) \iff *(i + \text{arr}) \iff i[\text{arr}]$$

### When Arrays Do NOT Decay:
1. When passed as the operand of `sizeof(arr)` (returns total byte size: $N \times \text{sizeof}(\text{type})$).
2. When passed as the operand of the address-of operator `&arr` (returns pointer to whole array `int (*)[N]`).
3. When initialized as a string literal array (`char s[] = "hello"`).

---

## 5. Comparison: Stack Array vs Heap Allocated Array (`malloc`) vs VLA (C99)

| Metric / Dimension | Fixed Stack Array | Dynamic Heap Array (`malloc`) | Variable Length Array (VLA, C99) |
| :--- | :--- | :--- | :--- |
| **Allocation Site** | Call Stack Frame | Process Heap | Call Stack Frame |
| **Size Determination**| Compile-time constant | Runtime dynamic | Runtime dynamic |
| **Lifetime** | Enclosing block scope | Manual until `free()` | Enclosing block scope |
| **Overhead** | 0 allocation overhead | `malloc()` metadata + pointer lookup | Stack pointer adjustment (`rsp`) |
| **Risk Factor** | Stack overflow on large arrays | Memory leaks / dangling pointers | High risk of stack overflow on large runtime $N$ |

---

## 6. Common Pitfalls & Undefined Behaviors

### 1. Out-of-Bounds Buffer Overrun
C performs **no bounds checking** at runtime. Accessing index $-1$ or $\ge N$ writes/reads arbitrary memory:
```c
int buf[5];
buf[5] = 42; /* UNDEFINED BEHAVIOR: Memory corruption / stack smashing! */
```

### 2. Passing Arrays to Functions (Loss of Size)
When an array is passed as a function argument `void func(int arr[])`, it decays into a pointer `int *arr`. Inside `func()`, `sizeof(arr)` evaluates to `sizeof(int *)` (8 bytes on 64-bit OS), NOT the array size.
**Fix:** Always pass the array length as a separate `size_t len` argument.

---

## 7. Production Best Practices

- Always calculate array element counts using the macro:
  ```c
  #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
  ```
- Use `size_t` for all array indexing to eliminate sign-comparison warnings and prevent 32-bit integer overflow indexing bugs.

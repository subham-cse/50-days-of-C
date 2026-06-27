# Day 21: 2D Arrays and Matrices in C

## 1. Overview and Mental Model
A two-dimensional (2D) array in C is conceptually a grid of elements organized into rows and columns, like a spreadsheet table or a mathematical matrix. However, physical computer memory (RAM) is strictly linear and one-dimensional—an unbroken sequence of byte addresses from `0` to `2^N - 1`.

To bridge this conceptual gap, C implements 2D arrays as an **"array of arrays"**. When you declare `int matrix[R][C];`, you are defining an array containing `R` elements, where each element is itself an array of `C` integers.

### Real-World Analogy
Imagine an apartment building with multiple floors:
- The entire building is the 2D array: `apartment_complex[floors][units_per_floor]`.
- Floor 0 has units `[0][0], [0][1], [0][2]`.
- Floor 1 has units `[1][0], [1][1], [1][2]`.
- In architectural schematics, floors are stacked vertically. But if all apartments were laid out along a single continuous hallway on the ground, Floor 0's units would precede Floor 1's units. This is precisely how **Row-Major Order** serializes a grid into linear memory.

---

## 2. Deep Technical Dive & Memory Architecture

### Row-Major Ordering (The C Standard)
C standardizes **Row-Major Layout** (unlike Fortran or MATLAB, which use Column-Major Layout). In Row-Major order, entire rows are stored contiguously in memory:
Row 0 is placed first, followed immediately by Row 1, then Row 2, and so on.

#### Address Calculation Formula
Given an array `T arr[R][C]` where `T` is a type of size `sizeof(T)`:
$$\text{Address}(\text{arr}[i][j]) = \text{Base Address} + \big(i \times C + j\big) \times \text{sizeof}(T)$$

Notice that $R$ (number of rows) does not appear in the formula! To compute an element's address, the compiler only needs to know $C$ (the column width) and the element size. This explains why function parameters for 2D arrays must specify column bounds: `void func(int arr[][COLUMNS]);`.

### ASCII Memory Representation

Given `int grid[3][4]` located at base address `0x1000` (assuming 4-byte `int`):

```text
Logical View (Matrix):
             Col 0      Col 1      Col 2      Col 3
           +----------+----------+----------+----------+
    Row 0  |  (0,0)   |  (0,1)   |  (0,2)   |  (0,3)   |
           +----------+----------+----------+----------+
    Row 1  |  (1,0)   |  (1,1)   |  (1,2)   |  (1,3)   |
           +----------+----------+----------+----------+
    Row 2  |  (2,0)   |  (2,1)   |  (2,2)   |  (2,3)   |
           +----------+----------+----------+----------+

Physical Memory Layout (Linear Row-Major):
+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
| (0,0)  | (0,1)  | (0,2)  | (0,3)  | (1,0)  | (1,1)  | (1,2)  | (1,3)  | (2,0)  | (2,1)  | (2,2)  | (2,3)  |
+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+
0x1000   0x1004   0x1008   0x100C   0x1010   0x1014   0x1018   0x101C   0x1020   0x1024   0x1028   0x102C
|<----------- Row 0 --------------->|<----------- Row 1 --------------->|<----------- Row 2 --------------->|
```

---

## 3. Syntax, Initialization, and Access

### Declaration & Static Initialization
```c
// Uninitialized (contains stack garbage values if local)
int mat[3][3];

// Fully initialized
int mat1[2][3] = {
    {10, 20, 30},
    {40, 50, 60}
};

// Flattened initializer (same binary result due to row-major order)
int mat2[2][3] = {10, 20, 30, 40, 50, 60};

// Partial initialization: missing elements default to 0
int mat3[3][3] = {
    {1, 2},      // row 0: 1, 2, 0
    {3}          // row 1: 3, 0, 0
                 // row 2: 0, 0, 0
};

// Row dimension inference (columns MUST still be specified)
int mat4[][3] = {
    {1, 2, 3},
    {4, 5, 6}
}; // Inferred as int[2][3]
```

### Pointer Decay & Dimensional Types
When inspecting `int arr[3][4]`:
- `arr` decays to `int (*)[4]` (pointer to an array of 4 ints), NOT `int**`!
- `*arr` or `arr[0]` is of type `int*` (points to element `(0, 0)`).
- `**arr` is an `int` with value at `(0, 0)`.
- `*(*(arr + i) + j)` is pointer-arithmetic equivalence to `arr[i][j]`.

---

## 4. Hardware Cache Locality & Performance ("Under the Hood")

Modern CPUs load memory into L1/L2/L3 cache lines (typically 64 bytes at a time). 
- **Row-wise Traversal (`for i ... for j`)**: Accesses memory sequentially (`arr[0][0], arr[0][1], arr[0][2]...`). Because adjacent elements reside in the same cache line, spatial locality is maximized, yielding nearly 100% L1 cache hits.
- **Column-wise Traversal (`for j ... for i`)**: Strides across memory by jumps of `C * sizeof(T)`. For large matrices, every iteration produces a cache miss, causing severe memory stall cycles.

```text
Row-wise traversal (Cache-Friendly):
[X] -> [X] -> [X] -> [X]  (Sequential contiguous access: Fast)

Column-wise traversal (Cache-Hostile):
[X] ---- Stride ----> [X] ---- Stride ----> [X] (Cache line evictions: Slow)
```

---

## 5. Comparative Analysis: Static 2D Array vs Array of Pointers vs Flat 1D Array

| Metric | Static 2D Array (`T[R][C]`) | Array of Pointers (`T* [R]`) | Flat 1D Array (`malloc(R*C*sizeof(T))`) |
| :--- | :--- | :--- | :--- |
| **Memory Allocation** | Single contiguous stack/data block | Non-contiguous row buffers + pointer array | Single contiguous heap block |
| **Indexing Syntax** | `arr[i][j]` | `arr[i][j]` | `arr[i * C + j]` |
| **Cache Locality** | Excellent (100% contiguous) | Poor (scattered rows in heap) | Excellent (100% contiguous) |
| **Pointer Overhead** | 0 bytes | `R * sizeof(void*)` bytes | 0 bytes |
| **Function Passing** | Needs fixed column width or C99 VLA | Direct `T**` passing | Direct `T*` with dimensions passed |

---

## 6. Common Pitfalls & Undefined Behaviors

1. **Incorrect Function Signature (`int**` vs `int[][C]`):**
   Passing a static `int arr[3][3]` to `void foo(int **ptr)` causes undefined behavior and segmentation faults because `arr` is a contiguous block, whereas `int**` expects a pointer to an array of pointer addresses.
2. **Column Omission in Prototypes:**
   `void func(int mat[][]);` is a compile-time error. The compiler cannot calculate `base + (i * C + j) * sizeof(int)` without knowing $C$.
3. **Out-of-Bounds Subscript Bleed:**
   In `int a[2][2] = {{1, 2}, {3, 4}};`, accessing `a[0][2]` will physically access `a[1][0]` in linear memory on most platforms, but it violates C object boundaries and is undefined behavior (UB).
4. **VLA Stack Overflow:**
   C99 variable-length arrays (`int arr[n][m]`) reside on the stack. Large dimensions ($N > 1000$) will trigger immediate stack exhaustion.

---

## 7. Production Best Practices & Complexity

- **Time Complexity:** Element lookup `arr[i][j]` is $\mathcal{O}(1)$ deterministic pointer arithmetic.
- **Space Complexity:** $\mathcal{O}(R \times C)$ total elements.
- Always loop outer-index = rows, inner-index = columns.
- For high-performance matrix computing (BLAS/LAPACK), favor flattened 1D allocations with inline index macros `#define IDX(r, c, cols) ((r) * (cols) + (c))` to guarantee continuous buffer locality and simplify interop.

# Day 22: Matrix Operations in C (Addition, Multiplication, Transposition)

## 1. Overview and Mathematical Foundations
Matrix operations are the foundational engine of graphics rendering, neural networks, computer vision, physics simulations, and scientific computing. In C, performing matrix operations involves implementing linear algebra algorithms over multi-dimensional or flattened linear buffers.

### Core Operations
1. **Matrix Addition / Subtraction**: Element-wise combination of two matrices of identical dimensions $(M \times N)$.
   $$C_{i,j} = A_{i,j} \pm B_{i,j}$$
2. **Matrix Transposition**: Swapping row indices with column indices to transform an $(M \times N)$ matrix into an $(N \times M)$ matrix.
   $$A^T_{j,i} = A_{i,j}$$
3. **Matrix Multiplication (Dot Product)**: Multiplying matrix $A$ of dimensions $(M \times K)$ by matrix $B$ of dimensions $(K \times N)$ to produce matrix $C$ of dimensions $(M \times N)$.
   $$C_{i,j} = \sum_{k=0}^{K-1} A_{i,k} \cdot B_{k,j}$$

---

## 2. Deep Technical Dive & Memory Flow

### Matrix Multiplication Architecture
For matrix multiplication $C = A \times B$:
- The number of columns in $A$ must strictly equal the number of rows in $B$ ($K$).
- Each element $C_{i,j}$ is computed as the vector dot product of Row $i$ of matrix $A$ and Column $j$ of matrix $B$.

```text
Matrix A (M x K)              Matrix B (K x N)                Matrix C (M x N)
+---+---+---+                  +---+---+---+                   +---+---+---+
| a | b | c | (Row i)    *     | d | . | . | (Col j)    =      | . | C | . | (Elem i,j)
+---+---+---+                  | e | . | . |                   +---+---+---+
                               | f | . | . |
                               +---+---+---+
Calculation: C[i][j] = (a * d) + (b * e) + (c * f)
```

### In-Place vs Out-of-Place Transposition
- **Square Matrix ($N \times N$)**: Can be transposed **in-place** with zero auxiliary memory by swapping elements across the main diagonal: `swap(&A[i][j], &A[j][i])` for all $i < j$.
- **Rectangular Matrix ($M \times N$ where $M \neq N$)**: Transposition reshapes the memory footprint and stride length. It typically requires an out-of-place destination buffer of size $(N \times M)$.

```text
Square Matrix In-Place Transpose (Main Diagonal Mirror):
Original:               After Swap (i < j):
[ 1   2   3 ]           [ 1   4   7 ]
[ 4   5   6 ]   ===>    [ 2   5   8 ]
[ 7   8   9 ]           [ 3   6   9 ]
  \   \   \
   Diagonal (unchanged)
```

---

## 3. Cache Performance in Matrix Algorithms ("Under the Hood")

### The Classic `ijk` vs `ikj` Loop Optimization
In naive matrix multiplication:
```c
// Classic naive order (ijk)
for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
        for (int k = 0; k < N; ++k) {
            C[i][j] += A[i][k] * B[k][j]; // B[k][j] causes large column strides in memory!
        }
    }
}
```
In the inner loop of `ijk`, `B[k][j]` steps through memory by strides of $N \times \text{sizeof}(\text{int})$, causing an L1/L2 cache miss on nearly every step when $N$ exceeds cache size.

**Optimized `ikj` Loop Order:**
```c
// Loop interchange optimization (ikj)
for (int i = 0; i < N; ++i) {
    for (int k = 0; k < N; ++k) {
        int r = A[i][k];
        for (int j = 0; j < N; ++j) {
            C[i][j] += r * B[k][j]; // Both C[i][j] and B[k][j] are accessed contiguously!
        }
    }
}
```
In `ikj`, both `B[k][j]` and `C[i][j]` are read/written sequentially along cache lines, resulting in up to a **$5\times - 10\times$ speedup** purely from cache spatial locality!

---

## 4. Algorithmic Complexity Comparison

| Operation | Input Dimensions | Output Dimensions | Time Complexity | Auxiliary Space | In-Place Supported? |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Matrix Addition** | $(M \times N), (M \times N)$ | $(M \times N)$ | $\mathcal{O}(M \times N)$ | $\mathcal{O}(1)$ or $\mathcal{O}(M \times N)$ | Yes |
| **Matrix Subtraction** | $(M \times N), (M \times N)$ | $(M \times N)$ | $\mathcal{O}(M \times N)$ | $\mathcal{O}(1)$ or $\mathcal{O}(M \times N)$ | Yes |
| **Square Transpose** | $(N \times N)$ | $(N \times N)$ | $\mathcal{O}(N^2)$ | $\mathcal{O}(1)$ | Yes (swap $i < j$) |
| **Rectangular Transpose** | $(M \times N)$ | $(N \times M)$ | $\mathcal{O}(M \times N)$ | $\mathcal{O}(M \times N)$ | No (general case) |
| **Standard Multiplication** | $(M \times K), (K \times N)$ | $(M \times N)$ | $\mathcal{O}(M \cdot K \cdot N)$ | $\mathcal{O}(M \times N)$ | No (aliasing risk) |
| **Strassen's Algorithm** | $(N \times N), (N \times N)$ | $(N \times N)$ | $\mathcal{O}(N^{2.807})$ | $\mathcal{O}(N^2)$ | No |

---

## 5. Common Pitfalls & Undefined Behaviors

1. **Dimensional Incompatibility:**
   Attempting multiplication when `cols(A) != rows(B)`. Accessing out-of-bound indices will corrupt the stack or read invalid memory.
2. **Buffer Aliasing in Multiplication:**
   Passing the same matrix as an operand and destination (`multiply(A, B, A)`) corrupts input rows before subsequent dot products are calculated. Always use a temporary buffer.
3. **Uninitialized Accumulator:**
   Failing to zero-initialize the destination matrix before performing `C[i][j] += ...` accumulates garbage memory values.
4. **VLA Dimension Mismatches:**
   Passing arrays with mismatched column dimensions to fixed-dimension parameter functions causes silent mathematical corruption due to stride miscalculations.

---

## 6. Production Best Practices

- Always validate matrix dimensional preconditions (`assert(cols_a == rows_b)`).
- Zero-initialize destination matrices before multi-pass accumulation.
- For high-performance code, utilize loop tiling (cache blocking) or SIMD vector intrinsics (AVX2/AVX-512).
- When passing arrays to reusable library functions, prefer explicit width parameters with C99 variable-length parameter ordering: `void multiply(int r1, int c1, const int a[r1][c1], int r2, int c2, const int b[r2][c2], int out[r1][c2]);`.

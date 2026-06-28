/**
 * ============================================================================
 * Day 22: Matrix Operations (Addition, Multiplication, Transposition)
 * File: 22_Matrix_Operations_code.c
 * 
 * Overview:
 * Production-grade implementation of fundamental linear algebra operations
 * on 2D matrices: element-wise addition/subtraction, in-place square matrix
 * transposition, out-of-place rectangular transposition, and cache-aware
 * matrix multiplication with dimensional validation.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 22_Matrix_Operations_code.c -o 22_Matrix_Operations_code
 * 
 * Complexity:
 * - Matrix Addition:       O(M * N) time, O(1) auxiliary space
 * - Square Transpose:      O(N^2) time,   O(1) auxiliary space (in-place)
 * - Matrix Multiplication: O(M * K * N) time, O(M * N) space
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_DIM 10

/**
 * Helper 1: Formatted Matrix Printer
 */
void print_matrix(const char *label, int rows, int cols, const int mat[MAX_DIM][MAX_DIM]) {
    printf("[+] %s (%dx%d):\n", label, rows, cols);
    for (int i = 0; i < rows; ++i) {
        printf("    | ");
        for (int j = 0; j < cols; ++j) {
            printf("%5d ", mat[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

/**
 * Helper 2: Matrix Addition: C = A + B
 * Precondition: A, B, and C must share identical dimensions (rows x cols).
 */
bool matrix_add(int rows, int cols,
                const int a[MAX_DIM][MAX_DIM],
                const int b[MAX_DIM][MAX_DIM],
                int out[MAX_DIM][MAX_DIM]) {
    if (rows <= 0 || cols <= 0 || rows > MAX_DIM || cols > MAX_DIM) {
        return false;
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            out[i][j] = a[i][j] + b[i][j];
        }
    }
    return true;
}

/**
 * Helper 3: In-Place Square Matrix Transposition
 * Transposes an (N x N) matrix using diagonal mirroring with O(1) auxiliary memory.
 */
void transpose_square_inplace(int n, int mat[MAX_DIM][MAX_DIM]) {
    assert(n > 0 && n <= MAX_DIM);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int temp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = temp;
        }
    }
}

/**
 * Helper 4: Rectangular Matrix Transposition: Out = In^T
 * Maps an (R x C) matrix to a (C x R) matrix.
 */
void transpose_rectangular(int r, int c, 
                           const int in[MAX_DIM][MAX_DIM], 
                           int out[MAX_DIM][MAX_DIM]) {
    assert(r > 0 && c > 0 && r <= MAX_DIM && c <= MAX_DIM);
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            out[j][i] = in[i][j];
        }
    }
}

/**
 * Helper 5: Matrix Multiplication: C = A * B
 * A is (rA x cA), B is (rB x cB).
 * Precondition: cA must equal rB. Output matrix C will be (rA x cB).
 */
bool matrix_multiply(int rA, int cA, const int A[MAX_DIM][MAX_DIM],
                     int rB, int cB, const int B[MAX_DIM][MAX_DIM],
                     int C[MAX_DIM][MAX_DIM]) {
    if (cA != rB || rA <= 0 || cA <= 0 || cB <= 0) {
        return false; // Dimensional mismatch
    }

    // Zero-initialize accumulator matrix
    for (int i = 0; i < rA; ++i) {
        for (int j = 0; j < cB; ++j) {
            C[i][j] = 0;
        }
    }

    // Cache-optimized ikj loop interchange
    for (int i = 0; i < rA; ++i) {
        for (int k = 0; k < cA; ++k) {
            int a_val = A[i][k];
            for (int j = 0; j < cB; ++j) {
                C[i][j] += a_val * B[k][j];
            }
        }
    }
    return true;
}

int main(void) {
    printf("============================================================\n");
    printf("   DAY 22: MATRIX OPERATIONS & LINEAR ALGEBRA ALGORITHMS    \n");
    printf("============================================================\n\n");

    // 1. Matrix Addition Demonstration
    int A[MAX_DIM][MAX_DIM] = {
        { 1,  2,  3},
        { 4,  5,  6}
    };
    int B[MAX_DIM][MAX_DIM] = {
        { 7,  8,  9},
        {10, 11, 12}
    };
    int Sum[MAX_DIM][MAX_DIM];
    
    print_matrix("Matrix A", 2, 3, A);
    print_matrix("Matrix B", 2, 3, B);

    if (matrix_add(2, 3, A, B, Sum)) {
        print_matrix("Sum (A + B)", 2, 3, Sum);
    }

    // 2. Rectangular Transposition Demonstration (2x3 -> 3x2)
    int A_Transposed[MAX_DIM][MAX_DIM];
    transpose_rectangular(2, 3, A, A_Transposed);
    print_matrix("Transposed Matrix A^T", 3, 2, A_Transposed);

    // 3. In-Place Square Matrix Transposition (3x3)
    int Square[MAX_DIM][MAX_DIM] = {
        {11, 12, 13},
        {21, 22, 23},
        {31, 32, 33}
    };
    print_matrix("Original Square Matrix", 3, 3, Square);
    transpose_square_inplace(3, Square);
    print_matrix("Square Matrix After In-Place Transpose", 3, 3, Square);

    // 4. Matrix Multiplication Demonstration: (2x3) * (3x2) = (2x2)
    int Product[MAX_DIM][MAX_DIM];
    printf("[+] Executing Matrix Multiplication (2x3) * (3x2)...\n");
    if (matrix_multiply(2, 3, A, 3, 2, A_Transposed, Product)) {
        print_matrix("Product Matrix (A * A^T)", 2, 2, Product);
    }

    // 5. Incompatible Dimension Handling Test
    printf("[+] Testing Invalid Dimension Handling ((2x3) * (2x2))...\n");
    bool status = matrix_multiply(2, 3, A, 2, 2, Product, Sum);
    if (!status) {
        printf("    [PASS] Correctly rejected multiplication with mismatched inner dimensions!\n");
    }

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}

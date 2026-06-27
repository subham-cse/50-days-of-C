/**
 * ============================================================================
 * Day 21: 2D Arrays and Matrices
 * File: 21_2D_Arrays_Matrices_code.c
 * 
 * Overview:
 * Comprehensive exploration of 2D array representation, row-major memory
 * mapping, pointer decay, safe matrix traversal, boundary verification,
 * and cache-conscious sequential processing.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 21_2D_Arrays_Matrices_code.c -o 21_2D_Arrays_Matrices_code
 * 
 * Complexity:
 * - Access Time: O(1)
 * - Traversal Time: O(Rows * Cols)
 * - Space Complexity: O(Rows * Cols) contiguous stack allocation
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define ROWS 3
#define COLS 4

/**
 * Helper 1: Print matrix elements along with exact physical memory addresses
 * Demonstrates contiguous row-major linear address progression.
 */
void print_matrix_with_addresses(const int matrix[ROWS][COLS]) {
    printf("[+] Logical 2D Grid with Linear Memory Addresses:\n");
    printf("    ------------------------------------------------------------------------\n");
    for (int r = 0; r < ROWS; ++r) {
        printf("    Row %d: ", r);
        for (int c = 0; c < COLS; ++c) {
            printf("[%d] @ %p  ", matrix[r][c], (const void*)&matrix[r][c]);
        }
        printf("\n");
    }
    printf("    ------------------------------------------------------------------------\n");
}

/**
 * Helper 2: Traverse matrix using pointer arithmetic to demonstrate row-major mapping formula:
 * Address(r, c) = Base + (r * COLS + c) * sizeof(int)
 */
void verify_row_major_flattening(const int *flat_ptr, int total_rows, int total_cols) {
    printf("[+] Verifying Pointer Arithmetic vs Direct Indexing:\n");
    for (int r = 0; r < total_rows; ++r) {
        for (int c = 0; c < total_cols; ++c) {
            int offset = r * total_cols + c;
            int value_via_offset = *(flat_ptr + offset);
            printf("    Element (%d, %d) -> Offset: %2d | Value: %3d\n", 
                   r, c, offset, value_via_offset);
        }
    }
}

/**
 * Helper 3: Calculate row sums and column sums efficiently
 */
void compute_matrix_statistics(const int matrix[ROWS][COLS], int row_sums[ROWS], int col_sums[COLS]) {
    // Zero-initialize accumulators
    for (int r = 0; r < ROWS; ++r) row_sums[r] = 0;
    for (int c = 0; c < COLS; ++c) col_sums[c] = 0;

    // Cache-friendly row-major traversal
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            row_sums[r] += matrix[r][c];
            col_sums[c] += matrix[r][c];
        }
    }
}

int main(void) {
    printf("============================================================\n");
    printf("   DAY 21: 2D ARRAYS, MATRICES & ROW-MAJOR ORDERING IN C    \n");
    printf("============================================================\n\n");

    // Initialize a 3x4 matrix
    int grid[ROWS][COLS] = {
        { 10,  20,  30,  40},
        { 50,  60,  70,  80},
        { 90, 100, 110, 120}
    };

    // 1. Display matrix values & addresses
    print_matrix_with_addresses(grid);
    printf("\n");

    // 2. Validate memory linearity and calculate byte stride between rows
    uintptr_t addr_row0 = (uintptr_t)&grid[0][0];
    uintptr_t addr_row1 = (uintptr_t)&grid[1][0];
    ptrdiff_t row_stride_bytes = addr_row1 - addr_row0;
    ptrdiff_t expected_bytes = COLS * sizeof(int);

    printf("[+] Row Stride Inspection:\n");
    printf("    Address of grid[0][0]: 0x%lx\n", (unsigned long)addr_row0);
    printf("    Address of grid[1][0]: 0x%lx\n", (unsigned long)addr_row1);
    printf("    Observed Stride:       %td bytes\n", row_stride_bytes);
    printf("    Formula (COLS * 4):    %td bytes\n", expected_bytes);
    if (row_stride_bytes == expected_bytes) {
        printf("    Status: [PASS] Row-major contiguous packing confirmed.\n\n");
    }

    // 3. Flattened pointer arithmetic access
    // Casting 2D array to flat 1D int pointer
    const int *flat_view = (const int *)grid;
    verify_row_major_flattening(flat_view, ROWS, COLS);
    printf("\n");

    // 4. Compute row and column sums
    int row_sums[ROWS];
    int col_sums[COLS];
    compute_matrix_statistics(grid, row_sums, col_sums);

    printf("[+] Matrix Aggregations:\n");
    for (int r = 0; r < ROWS; ++r) {
        printf("    Row %d Sum = %4d\n", r, row_sums[r]);
    }
    for (int c = 0; c < COLS; ++c) {
        printf("    Col %d Sum = %4d\n", c, col_sums[c]);
    }

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}

/**
 * ============================================================================
 * Day 41: Pointer to Pointer (Double Pointers)
 * File: 41_Pointer_To_Pointer_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 41_Pointer_To_Pointer_code.c -o 41_pointer_to_pointer
 *
 * Concepts Demonstrated:
 *   1. Modifying pointer targets across function boundaries (Pass-by-Reference).
 *   2. Dynamic allocation and deallocation of 2D jagged matrices.
 *   3. Safe defensive double pointer manipulation with NULL checking.
 *
 * Complexity:
 *   - Allocation: O(Rows * Cols) time, O(Rows * Cols + Rows * sizeof(ptr)) space.
 *   - Deallocation: O(Rows) time.
 *   - Element Lookup: O(1) time (2 indirection steps).
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * allocate_buffer - Allocates memory on the heap and assigns it to caller's pointer.
 * @param out_ptr: Address of the caller's pointer (double pointer).
 * @param count: Number of integer elements to allocate.
 * @return: true on successful allocation, false otherwise.
 */
bool allocate_buffer(int **out_ptr, size_t count) {
    if (out_ptr == NULL || count == 0) {
        return false;
    }

    *out_ptr = (int *)malloc(count * sizeof(int));
    if (*out_ptr == NULL) {
        return false;
    }

    /* Initialize allocated memory */
    for (size_t i = 0; i < count; ++i) {
        (*out_ptr)[i] = (int)((i + 1) * 10);
    }

    return true;
}

/**
 * free_buffer - Safely deallocates a buffer and zeroes out the caller's pointer.
 * @param out_ptr: Address of the caller's pointer.
 */
void free_buffer(int **out_ptr) {
    if (out_ptr != NULL && *out_ptr != NULL) {
        free(*out_ptr);
        *out_ptr = NULL; /* Prevent dangling pointer */
    }
}

/**
 * create_matrix - Dynamically creates an R x C 2D array using pointer-to-pointer.
 * @param rows: Number of rows.
 * @param cols: Number of columns.
 * @return: Pointer to an array of row pointers, or NULL on failure.
 */
int **create_matrix(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        return NULL;
    }

    /* Allocate array of row pointers */
    int **matrix = (int **)malloc(rows * sizeof(int *));
    if (matrix == NULL) {
        return NULL;
    }

    /* Allocate each individual row */
    for (size_t i = 0; i < rows; ++i) {
        matrix[i] = (int *)malloc(cols * sizeof(int));
        if (matrix[i] == NULL) {
            /* Unwind prior allocations on partial failure to prevent memory leaks */
            for (size_t k = 0; k < i; ++k) {
                free(matrix[k]);
            }
            free(matrix);
            return NULL;
        }

        /* Fill row values */
        for (size_t j = 0; j < cols; ++j) {
            matrix[i][j] = (int)((i + 1) * 100 + (j + 1));
        }
    }

    return matrix;
}

/**
 * destroy_matrix - Deallocates all rows and the top-level pointer array.
 * @param matrix_ptr: Address of the 2D matrix pointer.
 * @param rows: Number of rows in the matrix.
 */
void destroy_matrix(int ***matrix_ptr, size_t rows) {
    if (matrix_ptr == NULL || *matrix_ptr == NULL) {
        return;
    }

    int **matrix = *matrix_ptr;
    for (size_t i = 0; i < rows; ++i) {
        if (matrix[i] != NULL) {
            free(matrix[i]);
            matrix[i] = NULL;
        }
    }

    free(matrix);
    *matrix_ptr = NULL; /* Nullify caller reference */
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 41: POINTER TO POINTER (DOUBLE POINTERS)     \n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Basic Double Pointer Indirection
     * ------------------------------------------------------------------------ */
    int value = 42;
    int *ptr = &value;
    int **pptr = &ptr;

    printf("[1] Basic Indirection Verification:\n");
    printf("    Value Address (&value)  : %p\n", (void *)&value);
    printf("    Pointer Value (ptr)     : %p\n", (void *)ptr);
    printf("    Pointer Address (&ptr)  : %p\n", (void *)&ptr);
    printf("    Double Ptr Value (pptr) : %p\n", (void *)pptr);
    printf("    Direct Value via value  : %d\n", value);
    printf("    Value via *ptr          : %d\n", *ptr);
    printf("    Value via **pptr        : %d\n\n", **pptr);

    /* Mutating through double pointer */
    **pptr = 999;
    printf("    After **pptr = 999 -> value is now: %d (Expected: 999)\n\n", value);

    /* ------------------------------------------------------------------------
     * Section 2: Modifying Caller's Pointer via Double Pointer
     * ------------------------------------------------------------------------ */
    printf("[2] Modifying Caller's Pointer via allocate_buffer():\n");
    int *dynamic_array = NULL;
    const size_t array_len = 5;

    if (allocate_buffer(&dynamic_array, array_len)) {
        printf("    Buffer successfully allocated at %p\n", (void *)dynamic_array);
        printf("    Array Contents: ");
        for (size_t i = 0; i < array_len; ++i) {
            printf("%d ", dynamic_array[i]);
        }
        printf("\n");

        free_buffer(&dynamic_array);
        printf("    After free_buffer(), dynamic_array pointer is: %p\n\n", (void *)dynamic_array);
    } else {
        fprintf(stderr, "    Allocation failed!\n");
    }

    /* ------------------------------------------------------------------------
     * Section 3: Dynamic 2D Matrix (Jagged Array Model)
     * ------------------------------------------------------------------------ */
    printf("[3] Dynamic 2D Matrix Creation via int **:\n");
    const size_t rows = 3;
    const size_t cols = 4;
    int **matrix = create_matrix(rows, cols);

    if (matrix != NULL) {
        printf("    Matrix (%zux%zu) allocated successfully:\n", rows, cols);
        for (size_t r = 0; r < rows; ++r) {
            printf("    Row [%zu] at %p: ", r, (void *)matrix[r]);
            for (size_t c = 0; c < cols; ++c) {
                printf("%4d ", matrix[r][c]);
            }
            printf("\n");
        }

        destroy_matrix(&matrix, rows);
        printf("    Matrix successfully destroyed. matrix pointer is now: %p\n", (void *)matrix);
    } else {
        fprintf(stderr, "    Failed to allocate matrix!\n");
    }

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}

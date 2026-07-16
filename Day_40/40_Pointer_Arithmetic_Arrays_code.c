/**
 * ============================================================================
 * Day 40: Pointer Arithmetic, Array Decay & Multi-Dimensional Navigation
 * File: 40_Pointer_Arithmetic_Arrays_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 40_Pointer_Arithmetic_Arrays_code.c -o 40_Pointer_Arithmetic_Arrays_code
 * 
 * Description:
 *   Demonstrates pointer scaling across diverse data types, array subscript
 *   equivalence (arr[i] == *(arr + i) == i[arr]), pointer subtraction (ptrdiff_t),
 *   buffer iteration idioms (begin/end pointers), and 2D matrix traversal
 *   via pointer-to-array types.
 * 
 * Complexity:
 *   Time Complexity:  O(N) linear array and 2D matrix iteration
 *   Space Complexity: O(1) auxiliary memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Sums elements in a contiguous buffer using pointer iteration (Begin / End pointers).
 * 
 * @param start Pointer to first element.
 * @param end Pointer to one-past-the-last element.
 * @return Total sum of array elements.
 */
int64_t sum_buffer_pointer_idiom(const int32_t *start, const int32_t *end) {
    if (!start || !end || start > end) return 0;

    int64_t total = 0;
    const int32_t *cursor = start;

    while (cursor < end) {
        total += *cursor;
        cursor++; /* Advanced by sizeof(int32_t) bytes */
    }
    return total;
}

/**
 * @brief Demonstrates 2D matrix traversal using pointer-to-array.
 * 
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @param matrix Pointer to 2D array of fixed column size 4.
 */
void print_2d_matrix_pointers(size_t rows, const int32_t (*matrix)[4]) {
    printf("--- [2D Matrix Traversal via Pointer-to-Array 'int (*)[4]'] ---\n");
    for (size_t r = 0; r < rows; ++r) {
        /* Pointer to the start of current row */
        const int32_t *row_start = *(matrix + r);
        printf("Row %zu (Address: %p): [ ", r, (const void*)row_start);
        for (size_t c = 0; c < 4; ++c) {
            /* Accessing via pointer arithmetic */
            printf("%d ", *(row_start + c));
        }
        printf("]\n");
    }
    printf("\n");
}

/**
 * @brief Demonstrates byte scaling across different data types.
 */
void demonstrate_pointer_scaling(void) {
    char   c_arr[2] = { 'A', 'B' };
    short  s_arr[2] = { 10, 20 };
    int    i_arr[2] = { 100, 200 };
    double d_arr[2] = { 1.1, 2.2 };

    printf("--- [Pointer Arithmetic Scaling by Type Size] ---\n");
    printf("char*:   ptr0 = %p, ptr0+1 = %p (Delta: %td byte,  sizeof=%zu)\n",
           (void*)&c_arr[0], (void*)(&c_arr[0] + 1), 
           (uintptr_t)(&c_arr[0] + 1) - (uintptr_t)&c_arr[0], sizeof(char));

    printf("short*:  ptr0 = %p, ptr0+1 = %p (Delta: %td bytes, sizeof=%zu)\n",
           (void*)&s_arr[0], (void*)(&s_arr[0] + 1), 
           (uintptr_t)(&s_arr[0] + 1) - (uintptr_t)&s_arr[0], sizeof(short));

    printf("int*:    ptr0 = %p, ptr0+1 = %p (Delta: %td bytes, sizeof=%zu)\n",
           (void*)&i_arr[0], (void*)(&i_arr[0] + 1), 
           (uintptr_t)(&i_arr[0] + 1) - (uintptr_t)&i_arr[0], sizeof(int));

    printf("double*: ptr0 = %p, ptr0+1 = %p (Delta: %td bytes, sizeof=%zu)\n\n",
           (void*)&d_arr[0], (void*)(&d_arr[0] + 1), 
           (uintptr_t)(&d_arr[0] + 1) - (uintptr_t)&d_arr[0], sizeof(double));
}

/**
 * @brief Demonstrates subscript commutativity and ptrdiff_t subtraction.
 */
void demonstrate_subscript_and_ptrdiff(void) {
    int32_t numbers[] = { 10, 20, 30, 40, 50, 60, 70, 80 };
    size_t count = sizeof(numbers) / sizeof(numbers[0]);

    printf("--- [Array Subscript Commutativity & Pointer Subtraction] ---\n");
    
    /* Subscript equivalence test */
    size_t idx = 3;
    printf("Accessing element at index %zu:\n", idx);
    printf("  numbers[%zu]      = %d\n", idx, numbers[idx]);
    printf("  *(numbers + %zu)  = %d\n", idx, *(numbers + idx));
    printf("  *(%zu + numbers)  = %d\n", idx, *(idx + numbers));
    printf("  %zu[numbers]      = %d (Commutative syntax sugar!)\n\n", idx, idx[numbers]);

    /* Pointer subtraction (ptrdiff_t) */
    int32_t *p_first = &numbers[1];  /* Value 20 */
    int32_t *p_last  = &numbers[6];  /* Value 70 */
    ptrdiff_t diff = p_last - p_first;

    printf("Pointer Subtraction:\n");
    printf("  p_first points to index 1 (%p, val=%d)\n", (void*)p_first, *p_first);
    printf("  p_last  points to index 6 (%p, val=%d)\n", (void*)p_last, *p_last);
    printf("  p_last - p_first = %td elements (Byte delta: %td bytes)\n\n",
           diff, (uintptr_t)p_last - (uintptr_t)p_first);
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 40: POINTER ARITHMETIC & MULTI-DIMENSIONAL ARRAYS\n");
    printf("===============================================================\n\n");

    /* 1. Pointer Scaling */
    demonstrate_pointer_scaling();

    /* 2. Subscript duality and pointer subtraction */
    demonstrate_subscript_and_ptrdiff();

    /* 3. Begin / End Pointer Idiom Iteration */
    int32_t dataset[] = { 5, 15, 25, 35, 45, 55 };
    size_t data_len = sizeof(dataset) / sizeof(dataset[0]);

    printf("--- [Begin / End Pointer Iteration Idiom] ---\n");
    int64_t total_sum = sum_buffer_pointer_idiom(dataset, dataset + data_len);
    printf("Dataset elements: [ ");
    for (size_t i = 0; i < data_len; ++i) printf("%d ", dataset[i]);
    printf("]\nTotal Sum via pointer cursor: %lld\n\n", (long long)total_sum);

    /* 4. 2D Matrix traversal with pointer to array */
    int32_t matrix[3][4] = {
        { 11, 12, 13, 14 },
        { 21, 22, 23, 24 },
        { 31, 32, 33, 34 }
    };
    print_2d_matrix_pointers(3, matrix);

    return 0;
}

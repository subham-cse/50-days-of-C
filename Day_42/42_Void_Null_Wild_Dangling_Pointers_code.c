/**
 * ============================================================================
 * Day 42: Void, NULL, Wild, and Dangling Pointers
 * File: 42_Void_Null_Wild_Dangling_Pointers_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 42_Void_Null_Wild_Dangling_Pointers_code.c -o 42_pointers
 *
 * Concepts Demonstrated:
 *   1. Generic memory operations using void * with standard byte-casting.
 *   2. Safe NULL-pointer checking idioms and sentinel handling.
 *   3. Preventing dangling pointers using the SAFE_FREE idiom.
 *   4. Demonstrating type polymorphism via void pointer dispatch.
 *
 * Complexity:
 *   - Generic Print: O(1) time.
 *   - Generic Swap: O(size) time, O(size) stack space.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Safe Free Macro to eliminate dangling pointers */
#define SAFE_FREE(ptr) do { \
    free(ptr);              \
    (ptr) = NULL;           \
} while (0)

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

/**
 * generic_print - Prints a value pointed to by a void pointer based on type tag.
 * @param data: Pointer to data (void *).
 * @param type: Discriminator tag indicating underlying type.
 */
void generic_print(const void *data, DataType type) {
    if (data == NULL) {
        printf("[NULL]\n");
        return;
    }

    switch (type) {
        case TYPE_INT:
            printf("Integer Value: %d\n", *(const int *)data);
            break;
        case TYPE_FLOAT:
            printf("Float Value  : %.2f\n", *(const float *)data);
            break;
        case TYPE_STRING:
            printf("String Value : \"%s\"\n", (const char *)data);
            break;
        default:
            printf("Unknown Type\n");
            break;
    }
}

/**
 * generic_swap - Swaps two memory blocks of arbitrary type using byte-wise copy.
 * @param a: First memory address.
 * @param b: Second memory address.
 * @param size: Size in bytes of each element.
 */
void generic_swap(void *a, void *b, size_t size) {
    if (a == NULL || b == NULL || size == 0 || a == b) {
        return;
    }

    /* Allocate small stack buffer or heap buffer depending on size */
    unsigned char temp_buffer[256];
    void *temp = temp_buffer;
    bool heap_allocated = false;

    if (size > sizeof(temp_buffer)) {
        temp = malloc(size);
        if (temp == NULL) {
            fprintf(stderr, "Error: Memory allocation failed during swap.\n");
            return;
        }
        heap_allocated = true;
    }

    /* Byte-wise copy (C99 standard compliant void * memory handling) */
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);

    if (heap_allocated) {
        free(temp);
    }
}

/**
 * simulate_dangling_prevention - Demonstrates safe allocation and deallocation.
 */
void simulate_dangling_prevention(void) {
    printf("[Demonstrating Dangling Pointer Mitigation]\n");

    int *dynamic_val = (int *)malloc(sizeof(int));
    if (dynamic_val == NULL) {
        fprintf(stderr, "Allocation error.\n");
        return;
    }

    *dynamic_val = 1337;
    printf("  1. Allocated dynamic_val at %p, value = %d\n", (void *)dynamic_val, *dynamic_val);

    /* Safe free resets pointer to NULL immediately */
    SAFE_FREE(dynamic_val);
    printf("  2. Called SAFE_FREE(dynamic_val). Pointer is now: %p\n", (void *)dynamic_val);

    /* Subsequent checks protect against Use-After-Free */
    if (dynamic_val == NULL) {
        printf("  3. Defensive Check: dynamic_val is safely NULL; dereference prevented!\n\n");
    }
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 42: VOID, NULL, WILD & DANGLING POINTERS     \n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Generic Void Pointers
     * ------------------------------------------------------------------------ */
    printf("[1] Generic Void Pointer Demuxing:\n");
    int i_val = 42;
    float f_val = 3.14159f;
    char str_val[] = "Mastering C Programming";

    generic_print(&i_val, TYPE_INT);
    generic_print(&f_val, TYPE_FLOAT);
    generic_print(str_val, TYPE_STRING);
    generic_print(NULL, TYPE_INT);
    printf("\n");

    /* ------------------------------------------------------------------------
     * Section 2: Generic Swapping via Byte Pointers
     * ------------------------------------------------------------------------ */
    printf("[2] Generic Memory Swap Function:\n");
    int x = 100, y = 200;
    printf("  Before swap (int)  : x = %d, y = %d\n", x, y);
    generic_swap(&x, &y, sizeof(int));
    printf("  After swap (int)   : x = %d, y = %d\n", x, y);

    double d1 = 12.34, d2 = 56.78;
    printf("  Before swap (double): d1 = %.2f, d2 = %.2f\n", d1, d2);
    generic_swap(&d1, &d2, sizeof(double));
    printf("  After swap (double) : d1 = %.2f, d2 = %.2f\n\n", d1, d2);

    /* ------------------------------------------------------------------------
     * Section 3: Safe Pointer Lifetime Management
     * ------------------------------------------------------------------------ */
    simulate_dangling_prevention();

    /* ------------------------------------------------------------------------
     * Section 4: Wild Pointer Initialization Best Practice
     * ------------------------------------------------------------------------ */
    printf("[4] Initializing Pointers To NULL By Default:\n");
    /* Best Practice: Always initialize unassigned pointers to NULL */
    int *clean_ptr = NULL;
    if (clean_ptr == NULL) {
        printf("  clean_ptr safely verified as unassigned (NULL).\n");
    }

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}

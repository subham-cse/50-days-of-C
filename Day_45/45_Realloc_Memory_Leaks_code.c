/**
 * ============================================================================
 * Day 45: Dynamic Memory Resizing with realloc() & Memory Leak Prevention
 * File: 45_Realloc_Memory_Leaks_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 45_Realloc_Memory_Leaks_code.c -o 45_realloc_vector
 *
 * Concepts Demonstrated:
 *   1. Implementation of a production-grade dynamic integer vector.
 *   2. Safe realloc pattern using temporary pointer verification.
 *   3. Geometric capacity expansion (2x) for amortized O(1) push operations.
 *   4. Safe resource cleanup with zero memory leaks.
 *
 * Complexity:
 *   - Push Back: O(1) amortized time, O(N) worst-case on resize.
 *   - Shrink to Fit: O(N) time.
 *   - Space: O(Capacity) heap space.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 4

/* Dynamic Vector Data Structure */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} IntVector;

/**
 * vector_init - Initializes a new dynamic vector with base capacity.
 * @return: Pointer to initialized vector, or NULL on failure.
 */
IntVector *vector_init(void) {
    IntVector *vec = (IntVector *)malloc(sizeof(IntVector));
    if (vec == NULL) {
        return NULL;
    }

    vec->data = (int *)malloc(INITIAL_CAPACITY * sizeof(int));
    if (vec->data == NULL) {
        free(vec);
        return NULL;
    }

    vec->size = 0;
    vec->capacity = INITIAL_CAPACITY;
    return vec;
}

/**
 * vector_push - Appends an element, doubling capacity if full.
 * @param vec: Target vector.
 * @param value: Element to insert.
 * @return: true on success, false on allocation failure.
 */
bool vector_push(IntVector *vec, int value) {
    if (vec == NULL) {
        return false;
    }

    /* Check if capacity expansion is required */
    if (vec->size >= vec->capacity) {
        size_t new_capacity = vec->capacity * 2;
        
        /* Guard against size_t integer overflow */
        if (new_capacity < vec->capacity || new_capacity > ((size_t)-1 / sizeof(int))) {
            return false;
        }

        /* SAFE REALLOC PATTERN: Use temporary pointer */
        int *temp = (int *)realloc(vec->data, new_capacity * sizeof(int));
        if (temp == NULL) {
            /* Original memory in vec->data remains valid and intact */
            fprintf(stderr, "Error: realloc failed to expand vector capacity.\n");
            return false;
        }

        /* Update pointers only after verifying success */
        vec->data = temp;
        vec->capacity = new_capacity;
    }

    vec->data[vec->size++] = value;
    return true;
}

/**
 * vector_shrink_to_fit - Shrinks capacity to match exact current size.
 * @param vec: Target vector.
 * @return: true on success or if no shrink needed, false on realloc failure.
 */
bool vector_shrink_to_fit(IntVector *vec) {
    if (vec == NULL || vec->size == vec->capacity) {
        return true;
    }

    size_t target_capacity = (vec->size == 0) ? 1 : vec->size;
    int *temp = (int *)realloc(vec->data, target_capacity * sizeof(int));
    if (temp == NULL) {
        return false;
    }

    vec->data = temp;
    vec->capacity = target_capacity;
    return true;
}

/**
 * vector_free - Completely releases all vector heap memory.
 */
void vector_free(IntVector *vec) {
    if (vec == NULL) {
        return;
    }
    if (vec->data != NULL) {
        free(vec->data);
        vec->data = NULL;
    }
    free(vec);
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 45: DYNAMIC RESIZING (REALLOC & SAFE LEAKS)  \n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Dynamic Vector Operations & Geometric Growth
     * ------------------------------------------------------------------------ */
    printf("[1] Initializing Vector and Inserting Elements:\n");
    IntVector *vec = vector_init();
    if (vec == NULL) {
        fprintf(stderr, "Failed to initialize vector.\n");
        return 1;
    }

    printf("    Initial State: Size = %zu, Capacity = %zu, Base Pointer = %p\n",
           vec->size, vec->capacity, (void *)vec->data);

    /* Push 10 elements to trigger multiple realloc expansions (4 -> 8 -> 16) */
    for (int i = 1; i <= 10; ++i) {
        size_t cap_before = vec->capacity;
        void *ptr_before = (void *)vec->data;
        
        vector_push(vec, i * 100);

        if (vec->capacity != cap_before) {
            printf("    -> RESIZE TRIGGERED at size %zu: Capacity %zu -> %zu (Old Addr: %p, New Addr: %p)\n",
                   vec->size, cap_before, vec->capacity, ptr_before, (void *)vec->data);
        }
    }

    printf("\n    Vector Elements After 10 Pushes:\n    ");
    for (size_t i = 0; i < vec->size; ++i) {
        printf("[%zu]=%d ", i, vec->data[i]);
    }
    printf("\n    Current Metrics: Size = %zu, Capacity = %zu\n\n", vec->size, vec->capacity);

    /* ------------------------------------------------------------------------
     * Section 2: Shrinking Vector to Fit Payload
     * ------------------------------------------------------------------------ */
    printf("[2] Executing vector_shrink_to_fit():\n");
    if (vector_shrink_to_fit(vec)) {
        printf("    Shrunk successfully: Size = %zu, Capacity = %zu, Buffer Addr = %p\n",
               vec->size, vec->capacity, (void *)vec->data);
    }

    /* ------------------------------------------------------------------------
     * Section 3: Safe Memory Deallocation
     * ------------------------------------------------------------------------ */
    printf("\n[3] Freeing Vector and Validating Cleanup:\n");
    vector_free(vec);
    vec = NULL;
    printf("    Vector deallocated cleanly. Zero memory leaked.\n");

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}

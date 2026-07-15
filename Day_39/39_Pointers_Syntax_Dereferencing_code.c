/**
 * ============================================================================
 * Day 39: Pointers — Syntax, Addresses, Dereferencing & Indirection
 * File: 39_Pointers_Syntax_Dereferencing_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 39_Pointers_Syntax_Dereferencing_code.c -o 39_Pointers_Syntax_Dereferencing_code
 * 
 * Description:
 *   Demonstrates address-of operator (&), indirection dereferencing (*),
 *   multiple levels of indirection (double pointers), const qualifiers with
 *   pointers (pointer to const vs const pointer), and generic void* handling.
 * 
 * Complexity:
 *   Time Complexity:  O(1) memory dereferences
 *   Space Complexity: O(1) stack allocation
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Swaps two integer values using pointer dereferencing (Pass-by-Reference).
 * 
 * @param a Pointer to first integer.
 * @param b Pointer to second integer.
 */
void swap_integers(int *a, int *b) {
    if (!a || !b) return;
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Allocates and initializes an integer dynamically using a double pointer.
 * Demonstrates why double pointers are needed to modify the caller's pointer address.
 * 
 * @param pptr Pointer to the caller's integer pointer.
 * @param initial_value Value to assign.
 * @return true on allocation success, false otherwise.
 */
bool allocate_integer(int **pptr, int initial_value) {
    if (!pptr) return false;

    *pptr = (int *)malloc(sizeof(int));
    if (*pptr == NULL) {
        return false;
    }
    **pptr = initial_value;
    return true;
}

/**
 * @brief Generic memory inspector using void* and unsigned char*.
 * 
 * @param data Generic pointer to any memory object.
 * @param bytes Number of bytes to inspect.
 */
void inspect_generic_memory(const void *data, size_t bytes) {
    if (!data) return;
    const unsigned char *byte_ptr = (const unsigned char *)data;

    printf("Inspecting %zu bytes at address %p: [ ", bytes, data);
    for (size_t i = 0; i < bytes; ++i) {
        printf("%02X ", byte_ptr[i]);
    }
    printf("]\n");
}

/**
 * @brief Demonstrates the 4 quadrants of const correctness with pointers.
 */
void demonstrate_const_pointer_semantics(void) {
    int val1 = 10;
    int val2 = 20;

    printf("--- [Const Pointer Semantics Demonstration] ---\n");

    /* 1. Pointer to Const (Data is read-only, pointer can change) */
    const int *ptr_to_const = &val1;
    printf("1. const int *ptr_to_const:   points to val1 (%d)\n", *ptr_to_const);
    ptr_to_const = &val2; /* ALLOWED: pointer moves */
    printf("   Reassigned pointer to val2: points to val2 (%d)\n", *ptr_to_const);
    /* *ptr_to_const = 30; // COMPILATION ERROR: assignment of read-only location */

    /* 2. Const Pointer to Mutable Data (Pointer fixed, data can change) */
    int * const const_ptr = &val1;
    printf("2. int * const const_ptr:     points to val1 (%d)\n", *const_ptr);
    *const_ptr = 99; /* ALLOWED: modifies underlying val1 */
    printf("   Modified data via pointer: val1 is now %d\n", val1);
    /* const_ptr = &val2; // COMPILATION ERROR: assignment of read-only variable */

    /* 3. Const Pointer to Const Data (Everything immutable) */
    const int * const immutable_ptr = &val2;
    printf("3. const int * const imm_ptr: points to val2 (%d) [Completely Immutable]\n\n", *immutable_ptr);
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 39: POINTERS — SYNTAX, DEREFERENCING & INDIRECTION\n");
    printf("===============================================================\n\n");

    /* 1. Basic Address and Dereferencing */
    int x = 42;
    int *px = &x;
    int **ppx = &px;

    printf("[1] Memory Address & Indirection Topology:\n");
    printf("  Variable 'x' value:               %d\n", x);
    printf("  Variable 'x' address (&x):        %p\n", (void*)&x);
    printf("  Pointer 'px' value (holds &x):    %p\n", (void*)px);
    printf("  Pointer 'px' address (&px):       %p\n", (void*)&px);
    printf("  Dereference '*px':                %d\n", *px);
    printf("  Double pointer 'ppx' value:       %p\n", (void*)ppx);
    printf("  Double dereference '**ppx':       %d\n\n", **ppx);

    /* 2. Modifying values via pointer indirection (Pass-by-reference) */
    int num1 = 111, num2 = 999;
    printf("[2] Pass-by-Reference Swapping:\n");
    printf("  Before swap: num1 = %d, num2 = %d\n", num1, num2);
    swap_integers(&num1, &num2);
    printf("  After swap:  num1 = %d, num2 = %d\n\n", num1, num2);

    /* 3. Dynamic Allocation via Double Pointer */
    printf("[3] Double Pointer Heap Allocation:\n");
    int *heap_int = NULL;
    if (allocate_integer(&heap_int, 777)) {
        printf("  Successfully allocated int on heap at %p with value %d\n", 
               (void*)heap_int, *heap_int);
        free(heap_int);
        heap_int = NULL;
    }
    printf("\n");

    /* 4. Const pointer variations */
    printf("[4] Const-Correctness Rules:\n");
    demonstrate_const_pointer_semantics();

    /* 5. Generic void* memory inspection */
    printf("[5] Generic void* Pointer Inspection:\n");
    double test_double = 3.1415926535;
    uint32_t test_int32 = 0xDEADBEEF;

    inspect_generic_memory(&test_double, sizeof(test_double));
    inspect_generic_memory(&test_int32, sizeof(test_int32));

    return 0;
}

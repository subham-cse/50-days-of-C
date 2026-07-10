/**
 * ============================================================================
 * Day 34: Storage Classes — auto and register
 * File: 34_Auto_Register_Classes_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 34_Auto_Register_Classes_code.c -o 34_Auto_Register_Classes_code
 * 
 * Description:
 *   Demonstrates automatic storage duration, scope semantics of `auto`,
 *   performance timing of CPU `register` optimization hints vs stack variables,
 *   nested block shadow scoping, and safety guidelines for stack lifetime.
 * 
 * Complexity:
 *   Time Complexity:  O(1) variable access; O(N) loop benchmarking
 *   Space Complexity: O(1) stack allocation
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/**
 * @brief Demonstrates block scoping and automatic storage duration lifetime.
 */
void demonstrate_auto_lifetime_and_scoping(void) {
    printf("--- [Function Scope: Entry] ---\n");
    auto int outer_var = 100; /* Explicit 'auto' keyword */
    int implicit_auto = 200;  /* Implicit 'auto' */

    printf("Outer Scope: outer_var = %d (Addr: %p), implicit_auto = %d (Addr: %p)\n",
           outer_var, (void*)&outer_var, implicit_auto, (void*)&implicit_auto);

    {
        /* Nested Inner Block */
        auto int outer_var = 999; /* Shadows the outer variable */
        int inner_unique = 42;

        printf("  [Nested Block Entry]\n");
        printf("  Shadowed outer_var = %d (Addr: %p)\n", outer_var, (void*)&outer_var);
        printf("  Inner unique var   = %d (Addr: %p)\n", inner_unique, (void*)&inner_unique);
        printf("  [Nested Block Exit - inner variables deallocated from stack]\n");
    }

    printf("Outer Scope Restored: outer_var = %d (Addr: %p)\n\n",
           outer_var, (void*)&outer_var);
}

/**
 * @brief Benchmark function comparing register variable performance
 *        with standard automatic stack variable in a tight loop.
 * 
 * @param iterations Number of loop cycles.
 */
void benchmark_register_vs_auto(uint32_t iterations) {
    clock_t start, end;
    volatile uint64_t sink = 0; /* Volatile to prevent compiler from optimizing away the loop */

    printf("--- [Benchmarking Loop Performance: %u iterations] ---\n", iterations);

    /* 1. Benchmark standard auto variable */
    start = clock();
    auto uint64_t auto_accum = 0;
    for (auto uint32_t i = 0; i < iterations; ++i) {
        auto_accum += (i * 3) ^ (i >> 1);
    }
    sink = auto_accum;
    end = clock();
    double time_auto = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("1. Standard auto variable time:     %f seconds (Accum: %llu)\n", 
           time_auto, (unsigned long long)sink);

    /* 2. Benchmark register hinted variable */
    start = clock();
    register uint64_t reg_accum = 0;
    for (register uint32_t j = 0; j < iterations; ++j) {
        reg_accum += (j * 3) ^ (j >> 1);
    }
    sink = reg_accum;
    end = clock();
    double time_reg = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("2. Register hinted variable time:   %f seconds (Accum: %llu)\n", 
           time_reg, (unsigned long long)sink);

    printf("Benchmark comparison finished.\n\n");
}

/**
 * @brief Demonstrates register variable constraint rules.
 */
void demonstrate_register_constraints(register int param_reg) {
    register int reg_counter = param_reg * 2;

    printf("--- [Register Variable Constraints] ---\n");
    printf("Register parameter val: %d\n", param_reg);
    printf("Register local counter: %d\n", reg_counter);

    /* 
     * COMPILATION RULE DEMONSTRATION:
     * Uncommenting the following line would cause GCC compilation error:
     * "error: address of register variable requested"
     * 
     * int *ptr = &reg_counter;
     */
    printf("Note: Address-of (&reg_counter) is illegal in ANSI C for register variables.\n\n");
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 34: STORAGE CLASSES — AUTO AND REGISTER\n");
    printf("===============================================================\n\n");

    /* 1. Auto lifetime and shadow scoping */
    printf("[1] Automatic Duration & Nested Scoping Demonstration:\n");
    demonstrate_auto_lifetime_and_scoping();

    /* 2. Register constraints demonstration */
    printf("[2] Register Storage Class Properties:\n");
    demonstrate_register_constraints(25);

    /* 3. Performance Benchmark */
    printf("[3] Tight Loop Register vs Stack Benchmark:\n");
    benchmark_register_vs_auto(50000000);

    return 0;
}

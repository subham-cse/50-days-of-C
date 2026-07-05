/**
 * ============================================================================
 * Day 29: Recursion Fundamentals & Factorial Computation
 * File: 29_Recursion_Factorial_code.c
 * 
 * Overview:
 * Production-grade demonstration of recursion mechanics:
 * 1. Standard head/linear recursion with call stack trace visualization.
 * 2. Tail-recursive factorial with accumulator (TCO friendly).
 * 3. Fast iterative fallback with overflow bounds validation (N <= 20).
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 29_Recursion_Factorial_code.c -o 29_Recursion_Factorial_code
 * 
 * Complexity:
 * - Time: O(N) linear operations
 * - Space: O(N) call stack frames (standard) vs O(1) space (tail/iterative)
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#define MAX_EXACT_FACTORIAL_INPUT 20 // 20! is the maximum value representable in uint64_t

/**
 * Helper 1: Standard Recursion with Call Trace Visualization
 * Prints entry, stack depth indentation, base case, and unwinding.
 */
uint64_t factorial_traced(uint32_t n, int depth) {
    // Visual indentation matching call stack depth
    for (int i = 0; i < depth; ++i) printf("    ");
    printf("--> Entering factorial_traced(n = %u) [Frame Depth: %d]\n", n, depth);

    // Base Case
    if (n <= 1) {
        for (int i = 0; i < depth; ++i) printf("    ");
        printf("<-- Base Case Hit (n = %u)! Returning 1\n", n);
        return 1;
    }

    // Recursive Step
    uint64_t sub_result = factorial_traced(n - 1, depth + 1);
    uint64_t total = (uint64_t)n * sub_result;

    for (int i = 0; i < depth; ++i) printf("    ");
    printf("<-- Unwinding: %u * %" PRIu64 " = %" PRIu64 " [Popping Frame %d]\n", 
           n, sub_result, total, depth);

    return total;
}

/**
 * Helper 2: Tail Recursive Factorial Helper
 * Enables Tail Call Optimization (TCO) by maintaining an accumulator.
 */
static uint64_t factorial_tail_internal(uint32_t n, uint64_t accumulator) {
    if (n <= 1) {
        return accumulator;
    }
    return factorial_tail_internal(n - 1, n * accumulator);
}

/**
 * Public wrapper for tail recursive factorial with overflow guard
 */
bool factorial_tail(uint32_t n, uint64_t *out_result) {
    if (!out_result || n > MAX_EXACT_FACTORIAL_INPUT) {
        return false; // Prevent arithmetic overflow or null dereference
    }
    *out_result = factorial_tail_internal(n, 1);
    return true;
}

/**
 * Helper 3: Iterative Factorial (Baseline O(1) Space Reference)
 */
bool factorial_iterative(uint32_t n, uint64_t *out_result) {
    if (!out_result || n > MAX_EXACT_FACTORIAL_INPUT) {
        return false;
    }
    uint64_t product = 1;
    for (uint32_t i = 2; i <= n; ++i) {
        product *= i;
    }
    *out_result = product;
    return true;
}

int main(void) {
    printf("============================================================\n");
    printf("   DAY 29: RECURSION MECHANICS & FACTORIAL COMPUTATION      \n");
    printf("============================================================\n\n");

    // 1. Trace visualization of Call Stack for n = 4
    printf("[+] Detailed Call Stack Trace for factorial(4):\n");
    printf("    --------------------------------------------------------\n");
    uint64_t traced_res = factorial_traced(4, 0);
    printf("    --------------------------------------------------------\n");
    printf("    Final Result: 4! = %" PRIu64 "\n\n", traced_res);

    // 2. Testing Factorial Sequence from 0! to 10!
    printf("[+] Factorial Sequence (0! to 10!) comparing Tail vs Iterative:\n");
    printf("    N  | Tail Recursive        | Iterative             | Status\n");
    printf("    ---+-----------------------+-----------------------+--------\n");
    for (uint32_t i = 0; i <= 10; ++i) {
        uint64_t res_tail = 0, res_iter = 0;
        factorial_tail(i, &res_tail);
        factorial_iterative(i, &res_iter);

        printf("    %2u | %-21" PRIu64 " | %-21" PRIu64 " | %s\n", 
               i, res_tail, res_iter, (res_tail == res_iter) ? "[MATCH]" : "[MISMATCH]");
    }
    printf("\n");

    // 3. Overflow Protection and Maximum Bound (20! vs 21!)
    uint64_t max_fact = 0;
    if (factorial_tail(20, &max_fact)) {
        printf("[+] Maximum 64-bit Factorial Bound (20!):\n");
        printf("    20! = %" PRIu64 "\n", max_fact);
    }

    printf("[+] Testing Overflow Protection on 21! (Requires > 64 bits):\n");
    uint64_t overflow_res = 0;
    bool status_overflow = factorial_tail(21, &overflow_res);
    printf("    Execution Status: %s\n", status_overflow ? "CALCULATED" : "[REJECTED SAFELY]");

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}

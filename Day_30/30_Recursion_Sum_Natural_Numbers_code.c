/**
 * ============================================================================
 * Day 30: Recursion: Sum of Natural Numbers & Algorithmic Benchmarks
 * File: 30_Recursion_Sum_Natural_Numbers_code.c
 * 
 * Overview:
 * Comprehensive comparison of sum-of-natural-numbers implementations:
 * 1. Standard linear recursion with call tree depth tracking.
 * 2. Tail-recursive state accumulator (O(1) stack with TCO).
 * 3. Fast iterative accumulator.
 * 4. Constant-time O(1) Gauss closed-form mathematical solution with overflow bounds.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 30_Recursion_Sum_Natural_Numbers_code.c -o 30_Recursion_Sum_Natural_Numbers_code
 * 
 * Complexity:
 * - Standard Recursive: O(N) time, O(N) stack memory
 * - Tail Recursive:     O(N) time, O(1) stack memory (optimized)
 * - Gauss Closed-Form:  O(1) time, O(1) space
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#define MAX_SAFE_GAUSS_N 4294967295ULL // Fits within uint64_t without product overflow

/**
 * Helper 1: Standard Linear Recursion
 */
uint64_t sum_natural_recursive(uint32_t n) {
    if (n == 0) {
        return 0;
    }
    return (uint64_t)n + sum_natural_recursive(n - 1);
}

/**
 * Helper 2: Tail-Recursive Internal Worker
 */
static uint64_t sum_natural_tail_internal(uint32_t n, uint64_t accumulator) {
    if (n == 0) {
        return accumulator;
    }
    return sum_natural_tail_internal(n - 1, accumulator + n);
}

/**
 * Public wrapper for tail recursive sum
 */
uint64_t sum_natural_tail(uint32_t n) {
    return sum_natural_tail_internal(n, 0);
}

/**
 * Helper 3: Iterative Loop
 */
uint64_t sum_natural_iterative(uint32_t n) {
    uint64_t total = 0;
    for (uint32_t i = 1; i <= n; ++i) {
        total += i;
    }
    return total;
}

/**
 * Helper 4: Gauss Closed-Form Formula: S(N) = (N * (N + 1)) / 2
 * Runs in O(1) constant time with zero iteration.
 */
bool sum_natural_gauss(uint64_t n, uint64_t *out_sum) {
    if (!out_sum) return false;

    // Prevent uint64_t overflow during multiplication (n * (n + 1))
    // Max safe n where n * (n + 1) does not wrap around UINT64_MAX: ~6,074,000,999
    if (n > 6074000999ULL) {
        return false;
    }

    if (n % 2 == 0) {
        *out_sum = (n / 2) * (n + 1);
    } else {
        *out_sum = n * ((n + 1) / 2);
    }
    return true;
}

int main(void) {
    printf("============================================================\n");
    printf("   DAY 30: RECURSION, TAIL-CALLS & GAUSS' CLOSED-FORM MATH  \n");
    printf("============================================================\n\n");

    // 1. Comparing all 4 approaches on moderate inputs
    uint32_t test_inputs[] = {5, 10, 100, 1000};
    size_t num_tests = sizeof(test_inputs) / sizeof(test_inputs[0]);

    printf("[+] Comparative Evaluation Across Paradigms:\n");
    printf("     N   | Recursive     | Tail-Rec      | Iterative     | Gauss O(1)    | Status\n");
    printf(" --------+---------------+---------------+---------------+---------------+--------\n");

    for (size_t i = 0; i < num_tests; ++i) {
        uint32_t n = test_inputs[i];
        uint64_t r_rec = sum_natural_recursive(n);
        uint64_t r_tail = sum_natural_tail(n);
        uint64_t r_iter = sum_natural_iterative(n);
        uint64_t r_gauss = 0;
        sum_natural_gauss(n, &r_gauss);

        bool all_match = (r_rec == r_tail) && (r_tail == r_iter) && (r_iter == r_gauss);

        printf("  %6u | %-13" PRIu64 " | %-13" PRIu64 " | %-13" PRIu64 " | %-13" PRIu64 " | %s\n",
               n, r_rec, r_tail, r_iter, r_gauss, all_match ? "[MATCH]" : "[ERROR]");
    }
    printf("\n");

    // 2. High-Scale Demonstration where Recursion would cause Stack Overflow
    uint64_t large_n = 1000000000ULL; // 1 Billion (Recursion would crash stack)
    uint64_t large_sum = 0;
    printf("[+] High Scale Test with N = %" PRIu64 " (1 Billion):\n", large_n);
    printf("    Note: Standard recursion would require 1 billion stack frames (~32 GB)!\n");

    if (sum_natural_gauss(large_n, &large_sum)) {
        printf("    Computed instantaneously via Gauss O(1): %" PRIu64 "\n\n", large_sum);
    }

    // 3. Mathematical Identity Check
    // S(100) = 5050 (Famous Gauss childhood classroom calculation)
    uint64_t gauss_100 = 0;
    sum_natural_gauss(100, &gauss_100);
    printf("[+] Historic Verification: Sum(1..100) = %" PRIu64 " (Expected: 5050)\n", gauss_100);

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}

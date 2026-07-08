/**
 * ============================================================================
 * Day 32: Recursive Digit Sum, Digit Count & Digital Root
 * File: 32_Recursion_Digit_Sum_Root_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 32_Recursion_Digit_Sum_Root_code.c -o 32_Recursion_Digit_Sum_Root_code
 * 
 * Description:
 *   Demonstrates recursive digit summation, tail-recursive digit count,
 *   recursive digital root reduction, and constant-time O(1) digital root
 *   calculation based on modulo 9 congruence.
 * 
 * Complexity:
 *   Time Complexity:  O(log10(n)) for digit sum/count; O(1) for math digital root
 *   Space Complexity: O(log10(n)) stack depth (unoptimized); O(1) tail-recursive
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

/**
 * @brief Standard recursive digit sum.
 * Recurrence: S(n) = (n % 10) + S(n / 10)
 * 
 * @param n Positive integer.
 * @return Sum of digits.
 */
uint64_t sum_digits_recursive(uint64_t n) {
    if (n == 0) {
        return 0;
    }
    return (n % 10) + sum_digits_recursive(n / 10);
}

/**
 * @brief Tail-recursive digit sum using an accumulator.
 * Optimized by C compilers into a tight register loop.
 * 
 * @param n Positive integer.
 * @param acc Accumulated sum.
 * @return Total sum of digits.
 */
uint64_t sum_digits_tail(uint64_t n, uint64_t acc) {
    if (n == 0) {
        return acc;
    }
    return sum_digits_tail(n / 10, acc + (n % 10));
}

/**
 * @brief Recursive digit counter.
 * 
 * @param n Positive integer.
 * @return Number of digits in n.
 */
uint32_t count_digits_recursive(uint64_t n) {
    if (n < 10) {
        return 1;
    }
    return 1 + count_digits_recursive(n / 10);
}

/**
 * @brief Recursive digital root.
 * Repeatedly sums digits until a single-digit integer is reached.
 * 
 * @param n Positive integer.
 * @return Digital root in range [0, 9].
 */
uint32_t digital_root_recursive(uint64_t n) {
    if (n < 10) {
        return (uint32_t)n;
    }
    uint64_t sum = sum_digits_recursive(n);
    return digital_root_recursive(sum);
}

/**
 * @brief O(1) Digital Root calculation via Congruence Formula.
 * Mathematical property: dr(n) = 1 + ((n - 1) % 9) for n > 0.
 * 
 * @param n Non-negative integer.
 * @return Digital root in range [0, 9].
 */
uint32_t digital_root_formula(uint64_t n) {
    if (n == 0) {
        return 0;
    }
    return (uint32_t)(1 + ((n - 1) % 9));
}

/**
 * @brief Helper to trace recursive digit processing step-by-step.
 */
void trace_digit_sum(uint64_t n, int depth) {
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("-> sum_digits(%" PRIu64 ")\n", n);

    if (n == 0) {
        for (int i = 0; i < depth; ++i) {
            printf("  ");
        }
        printf("<- Base case: returns 0\n");
        return;
    }

    trace_digit_sum(n / 10, depth + 1);

    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("<- Returning (%" PRIu64 " %% 10 = %" PRIu64 ") + Subresult\n", n, n % 10);
}

int main(void) {
    printf("===============================================================\n");
    printf("       DAY 32: RECURSIVE DIGIT SUM & DIGITAL ROOT\n");
    printf("===============================================================\n\n");

    /* Test 1: Step-by-step trace of digit sum */
    uint64_t test_val1 = 9845;
    printf("[1] Recursive Digit Sum Execution Trace for %" PRIu64 ":\n", test_val1);
    trace_digit_sum(test_val1, 0);
    printf("Result (Standard): %" PRIu64 "\n", sum_digits_recursive(test_val1));
    printf("Result (Tail-Rec): %" PRIu64 "\n", sum_digits_tail(test_val1, 0));
    printf("Digit Count:       %u\n\n", count_digits_recursive(test_val1));

    /* Test 2: Digital Root Comparison (Recursive vs Closed Form) */
    uint64_t numbers[] = { 0, 5, 18, 493193, 999999999ULL, 123456789012345ULL };
    size_t count = sizeof(numbers) / sizeof(numbers[0]);

    printf("[2] Digital Root Comparison Across Diverse Values:\n");
    printf("%-22s %-12s %-15s %-15s %-8s\n", 
           "Number", "Digit Sum", "Recursive Root", "Formula Root", "Match?");
    printf("----------------------------------------------------------------------\n");

    for (size_t i = 0; i < count; ++i) {
        uint64_t num = numbers[i];
        uint64_t sum = sum_digits_recursive(num);
        uint32_t rec_root = digital_root_recursive(num);
        uint32_t form_root = digital_root_formula(num);

        printf("%-22" PRIu64 " %-12" PRIu64 " %-15u %-15u %-8s\n",
               num, sum, rec_root, form_root,
               (rec_root == form_root) ? "PASS" : "FAIL");
    }
    printf("\n");

    /* Test 3: Large 64-bit boundary testing */
    uint64_t large_val = 18446744073709551615ULL; /* UINT64_MAX */
    printf("[3] Boundary Test (UINT64_MAX):\n");
    printf("Value:        %" PRIu64 "\n", large_val);
    printf("Digit Sum:    %" PRIu64 "\n", sum_digits_tail(large_val, 0));
    printf("Digit Count:  %u\n", count_digits_recursive(large_val));
    printf("Digital Root: %u (Formula: %u)\n", 
           digital_root_recursive(large_val), digital_root_formula(large_val));

    return 0;
}

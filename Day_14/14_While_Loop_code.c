/**
 * ============================================================================
 * Day 14: While Loop & Indeterminate Iteration
 * Description : Production demonstration of entry-controlled while loops:
 *               Euclidean GCD algorithm, Collatz conjecture sequence analysis,
 *               and robust integer digit parsing.
 * Compilation : gcc -Wall -Wextra -std=c99 14_While_Loop_code.c -o 14_While_Loop_code
 * Execution   : ./14_While_Loop_code
 * 
 * Complexity  : GCD: O(log(min(A, B)))
 *               Digit Reverse: O(log10(N))
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Computes Greatest Common Divisor using the classical Euclidean while-loop algorithm.
 * @param a First positive integer.
 * @param b Second positive integer.
 * @return Greatest common divisor.
 */
uint64_t compute_gcd_euclidean(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

/**
 * @brief Evaluates the Collatz 3n+1 sequence length and peak value for a given seed.
 * @param n Starting positive integer.
 * @param out_peak Pointer to store the maximum value reached during sequence.
 * @param out_steps Pointer to store the total number of transition steps.
 */
void evaluate_collatz_conjecture(uint64_t n, uint64_t *out_peak, uint32_t *out_steps) {
    uint64_t current = n;
    uint64_t peak = n;
    uint32_t steps = 0;

    /* Loop continues while sequence has not converged to 1 */
    while (current > 1) {
        if ((current & 1) == 0) {
            /* Even: current / 2 */
            current = current / 2;
        } else {
            /* Odd: 3*current + 1 */
            current = 3 * current + 1;
        }

        if (current > peak) {
            peak = current;
        }
        steps++;
    }

    if (out_peak != NULL) *out_peak = peak;
    if (out_steps != NULL) *out_steps = steps;
}

/**
 * @brief Reverses an integer using base-10 modulo and division while loop.
 * @param num Input integer.
 * @return Numerically reversed integer.
 */
int64_t reverse_integer_digits(int64_t num) {
    int64_t reversed = 0;
    bool is_negative = (num < 0);
    if (is_negative) num = -num;

    while (num > 0) {
        int digit = num % 10;
        reversed = reversed * 10 + digit;
        num /= 10;
    }

    return is_negative ? -reversed : reversed;
}

int main(void) {
    printf("===================================================================\n");
    printf("             DAY 14: WHILE LOOP ITERATIVE ALGORITHMS               \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: Euclidean GCD */
    printf("[TEST SUITE 1: Euclidean GCD Algorithm]\n");
    struct { uint64_t a; uint64_t b; } gcd_pairs[] = {
        {1071, 462},
        {48, 18},
        {101, 103},
        {270, 192}
    };
    size_t num_pairs = sizeof(gcd_pairs) / sizeof(gcd_pairs[0]);

    for (size_t i = 0; i < num_pairs; ++i) {
        uint64_t g = compute_gcd_euclidean(gcd_pairs[i].a, gcd_pairs[i].b);
        printf("GCD(%lu, %lu) = %lu\n", gcd_pairs[i].a, gcd_pairs[i].b, g);
    }

    /* Test Suite 2: Collatz 3n+1 Sequence */
    printf("\n[TEST SUITE 2: Collatz 3n+1 Sequence Traversal]\n");
    uint64_t seeds[] = {6, 11, 27, 871};
    size_t num_seeds = sizeof(seeds) / sizeof(seeds[0]);

    for (size_t j = 0; j < num_seeds; ++j) {
        uint64_t peak = 0;
        uint32_t steps = 0;
        evaluate_collatz_conjecture(seeds[j], &peak, &steps);
        printf("Seed: %5lu | Steps to converge: %4u | Peak Value: %8lu\n",
               seeds[j], steps, peak);
    }

    /* Test Suite 3: Base-10 Integer Digit Reversal */
    printf("\n[TEST SUITE 3: Base-10 Digit Reversal]\n");
    int64_t test_ints[] = {1234567, 9870001, -45689, 0};
    size_t num_ints = sizeof(test_ints) / sizeof(test_ints[0]);

    for (size_t k = 0; k < num_ints; ++k) {
        int64_t rev = reverse_integer_digits(test_ints[k]);
        printf("Original: %10ld -> Reversed: %10ld\n", test_ints[k], rev);
    }

    printf("\n===================================================================\n");
    return 0;
}

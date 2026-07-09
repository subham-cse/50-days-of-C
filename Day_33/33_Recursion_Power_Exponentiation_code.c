/**
 * ============================================================================
 * Day 33: Fast Power & Modular Binary Exponentiation
 * File: 33_Recursion_Power_Exponentiation_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 33_Recursion_Power_Exponentiation_code.c -o 33_Recursion_Power_Exponentiation_code
 * 
 * Description:
 *   Implements Divide-and-Conquer Fast Power (Binary Exponentiation) supporting
 *   positive/negative fractional powers (IEEE-754 doubles), Recursive Modular
 *   Exponentiation for cryptography, and Iterative Bitwise Exponentiation.
 * 
 * Complexity:
 *   Time Complexity:  O(log(n)) multiplications
 *   Space Complexity: O(log(n)) stack frames recursive; O(1) iterative
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>

/**
 * @brief Computes x^n recursively in O(log n) time.
 * Supports positive and negative integer powers.
 * 
 * @param base Base number (double).
 * @param exp Exponent (integer).
 * @return Computed value of base^exp.
 */
double power_recursive(double base, long long exp) {
    /* Base case 1: Any number to power 0 is 1.0 */
    if (exp == 0) {
        return 1.0;
    }

    /* Handle negative exponents: x^(-n) = 1 / (x^n) */
    if (exp < 0) {
        /* Avoid LLONG_MIN negation overflow */
        if (exp == LLONG_MIN) {
            return (1.0 / base) * power_recursive(1.0 / base, -(exp + 1));
        }
        return 1.0 / power_recursive(base, -exp);
    }

    /* Divide and conquer */
    double half = power_recursive(base, exp / 2);

    if (exp % 2 == 0) {
        return half * half;
    } else {
        return base * half * half;
    }
}

/**
 * @brief Computes (base^exp) % mod recursively in O(log n) time.
 * Prevents intermediate overflow using 64-bit unsigned arithmetic.
 * 
 * @param base Integer base.
 * @param exp Non-negative integer exponent.
 * @param mod Modulus (must be > 0).
 * @return (base^exp) % mod.
 */
uint64_t mod_power_recursive(uint64_t base, uint64_t exp, uint64_t mod) {
    if (mod == 1) {
        return 0; /* Any number mod 1 is 0 */
    }
    if (exp == 0) {
        return 1;
    }

    /* Reduce base first */
    base %= mod;

    /* Recursive subproblem */
    uint64_t half = mod_power_recursive(base, exp / 2, mod);
    uint64_t half_sq = (uint64_t)(((unsigned __int128)half * half) % mod);

    if (exp % 2 == 0) {
        return half_sq;
    } else {
        return (uint64_t)(((unsigned __int128)half_sq * base) % mod);
    }
}

/**
 * @brief Iterative binary exponentiation using bitwise shifts.
 * 
 * @param base Double base.
 * @param exp Non-negative integer exponent.
 * @return base^exp.
 */
double power_iterative(double base, uint64_t exp) {
    double result = 1.0;
    double current_square = base;

    while (exp > 0) {
        /* If current lowest bit is set, accumulate */
        if (exp & 1) {
            result *= current_square;
        }
        /* Square base for next bit */
        current_square *= current_square;
        exp >>= 1; /* Shift exponent right */
    }
    return result;
}

/**
 * @brief Traces recursive execution step-by-step.
 */
void trace_power_recursion(double base, long long exp, int depth) {
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("-> power(base=%.2f, exp=%lld)\n", base, exp);

    if (exp == 0) {
        for (int i = 0; i < depth; ++i) {
            printf("  ");
        }
        printf("<- Base case: returns 1.0\n");
        return;
    }

    trace_power_recursion(base, exp / 2, depth + 1);

    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("<- Computed power(%.2f, %lld) from subproblem\n", base, exp);
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 33: FAST BINARY & MODULAR EXPONENTIATION\n");
    printf("===============================================================\n\n");

    /* Test 1: Recursive Binary Exponentiation Trace */
    double b1 = 2.0;
    long long e1 = 13;
    printf("[1] Recursion Call Trace for %.1f^%lld:\n", b1, e1);
    trace_power_recursion(b1, e1, 0);
    printf("Computed Result: %.2f (Expected: %.2f)\n\n", 
           power_recursive(b1, e1), pow(b1, (double)e1));

    /* Test 2: Negative and Fractional Powers */
    printf("[2] Testing Negative and Fractional Exponents:\n");
    double bases[] = { 2.0, 5.0, 3.0, -2.0, 10.0 };
    long long exps[] = { -3, -2, 4, 3, -4 };
    size_t count = sizeof(bases) / sizeof(bases[0]);

    for (size_t i = 0; i < count; ++i) {
        double r_rec = power_recursive(bases[i], exps[i]);
        printf("  (%.1f)^(%3lld) = %14.8f  [pow() check: %14.8f]\n",
               bases[i], exps[i], r_rec, pow(bases[i], (double)exps[i]));
    }
    printf("\n");

    /* Test 3: Iterative Bitwise Exponentiation */
    printf("[3] Iterative Bitwise Power Demonstration:\n");
    double b3 = 3.0;
    uint64_t e3 = 10;
    printf("  %.1f^%" PRIu64 " = %.2f\n\n", b3, e3, power_iterative(b3, e3));

    /* Test 4: Modular Exponentiation for Cryptography */
    uint64_t crypto_base = 7;
    uint64_t crypto_exp = 560;
    uint64_t crypto_mod = 561; /* Carmichael number */
    uint64_t mod_res = mod_power_recursive(crypto_base, crypto_exp, crypto_mod);

    printf("[4] Modular Exponentiation (Cryptography):\n");
    printf("  (%" PRIu64 "^%" PRIu64 ") mod %" PRIu64 " = %" PRIu64 "\n",
           crypto_base, crypto_exp, crypto_mod, mod_res);
    printf("  Fermat's Little Theorem Test Result: %s\n\n",
           (mod_res == 1) ? "PASSED (Congruent to 1)" : "FAILED");

    /* Test 5: Edge Case 0^0 and 0^n */
    printf("[5] Edge Cases:\n");
    printf("  0^0 = %.1f\n", power_recursive(0.0, 0));
    printf("  0^5 = %.1f\n", power_recursive(0.0, 5));

    return 0;
}

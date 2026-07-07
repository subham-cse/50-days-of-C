/**
 * ============================================================================
 * Day 31: Greatest Common Divisor (GCD) & Extended Euclidean Algorithm
 * File: 31_Recursion_GCD_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 31_Recursion_GCD_code.c -o 31_Recursion_GCD_code
 * 
 * Description:
 *   Implements classical recursive Euclidean GCD, iterative Euclidean GCD,
 *   Recursive Extended Euclidean Algorithm (computing Bézout coefficients),
 *   and Modular Multiplicative Inverse derivation.
 * 
 * Complexity:
 *   Time Complexity:  O(log(min(a, b)))
 *   Space Complexity: O(log(min(a, b))) recursive stack depth; O(1) iterative
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/**
 * @brief Computes GCD recursively using the classic Euclidean algorithm.
 * @param a First non-negative integer.
 * @param b Second non-negative integer.
 * @return Greatest Common Divisor of a and b.
 */
long long gcd_recursive(long long a, long long b) {
    if (b == 0) {
        return a;
    }
    return gcd_recursive(b, a % b);
}

/**
 * @brief Computes GCD iteratively (O(1) auxiliary stack space).
 * @param a First non-negative integer.
 * @param b Second non-negative integer.
 * @return Greatest Common Divisor of a and b.
 */
long long gcd_iterative(long long a, long long b) {
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/**
 * @brief Extended Euclidean Algorithm.
 * Computes integers x and y such that: a * x + b * y = gcd(a, b)
 * 
 * @param a Input coefficient a.
 * @param b Input coefficient b.
 * @param x Pointer to output Bézout coefficient for a.
 * @param y Pointer to output Bézout coefficient for b.
 * @return Greatest common divisor of a and b.
 */
long long gcd_extended(long long a, long long b, long long *x, long long *y) {
    /* Base case */
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }

    long long x1 = 0, y1 = 0;
    long long gcd = gcd_extended(b, a % b, &x1, &y1);

    /* Update x and y using results of recursive call */
    *x = y1;
    *y = x1 - (a / b) * y1;

    return gcd;
}

/**
 * @brief Computes modular multiplicative inverse of 'a' modulo 'm'.
 * Finds x such that (a * x) % m == 1.
 * 
 * @param a Number whose inverse is sought.
 * @param m Modulus.
 * @param inv Pointer to store the computed inverse.
 * @return true if inverse exists (i.e. gcd(a, m) == 1), false otherwise.
 */
bool modular_inverse(long long a, long long m, long long *inv) {
    long long x, y;
    long long g = gcd_extended(a, m, &x, &y);
    if (g != 1) {
        return false; /* Modular inverse does not exist */
    }
    /* In C, modulo can be negative; normalize to [0, m - 1] */
    *inv = (x % m + m) % m;
    return true;
}

/**
 * @brief Demonstrates recursive call trace for GCD computation.
 */
void trace_gcd_recursion(long long a, long long b, int depth) {
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("-> gcd_recursive(a=%lld, b=%lld)\n", a, b);

    if (b == 0) {
        for (int i = 0; i < depth; ++i) {
            printf("  ");
        }
        printf("<- Base case reached! Returning a = %lld\n", a);
        return;
    }

    trace_gcd_recursion(b, a % b, depth + 1);

    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("<- Returning result for gcd(%lld, %lld)\n", a, b);
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 31: RECURSIVE & EXTENDED EUCLIDEAN ALGORITHMS\n");
    printf("===============================================================\n\n");

    /* Test Pair 1: Basic GCD */
    long long a1 = 252, b1 = 105;
    printf("[1] Basic Euclidean GCD Demonstration:\n");
    printf("Computing gcd(%lld, %lld)...\n", a1, b1);
    trace_gcd_recursion(a1, b1, 0);
    printf("Result (Recursive) = %lld\n", gcd_recursive(a1, b1));
    printf("Result (Iterative) = %lld\n\n", gcd_iterative(a1, b1));

    /* Test Pair 2: Coprime numbers */
    long long a2 = 17, b2 = 31;
    printf("[2] Coprime Numbers Test:\n");
    printf("gcd(%lld, %lld) = %lld (Expected: 1)\n\n", a2, b2, gcd_recursive(a2, b2));

    /* Test Pair 3: Extended Euclidean Algorithm */
    long long a3 = 30, b3 = 20;
    long long x = 0, y = 0;
    long long g = gcd_extended(a3, b3, &x, &y);
    printf("[3] Extended Euclidean Algorithm (Bézout Identity):\n");
    printf("Equation: %lld * (x) + %lld * (y) = gcd(%lld, %lld)\n", a3, b3, a3, b3);
    printf("Computed: x = %lld, y = %lld, gcd = %lld\n", x, y, g);
    printf("Verification: %lld * (%lld) + %lld * (%lld) = %lld (Matches gcd: %s)\n\n",
           a3, x, b3, y, a3 * x + b3 * y,
           (a3 * x + b3 * y == g) ? "YES" : "NO");

    /* Test Pair 4: Modular Multiplicative Inverse */
    long long a4 = 3, m4 = 11;
    long long inv = 0;
    printf("[4] Modular Multiplicative Inverse Application:\n");
    if (modular_inverse(a4, m4, &inv)) {
        printf("Inverse of %lld modulo %lld is: %lld\n", a4, m4, inv);
        printf("Verification: (%lld * %lld) %% %lld = %lld\n\n",
               a4, inv, m4, (a4 * inv) % m4);
    } else {
        printf("Inverse of %lld modulo %lld does not exist.\n\n", a4, m4);
    }

    /* Test Pair 5: Large numbers & Edge cases */
    long long a5 = 1234567890LL, b5 = 9876543210LL;
    printf("[5] Large Number GCD:\n");
    printf("gcd(%lld, %lld) = %lld\n", a5, b5, gcd_recursive(a5, b5));

    return 0;
}

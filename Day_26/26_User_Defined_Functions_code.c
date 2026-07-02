/**
 * ============================================================================
 * Day 26: User-Defined Functions in C
 * File: 26_User_Defined_Functions_code.c
 * 
 * Overview:
 * In-depth demonstration of modular programming with user-defined functions:
 * prototypes, parameter validation, multiple return scenarios, pure math
 * computations, static helper functions, and safe data transformation.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 26_User_Defined_Functions_code.c -o 26_User_Defined_Functions_code
 * 
 * Complexity:
 * - is_prime: O(sqrt(N)) time, O(1) space
 * - compute_power: O(exp) time, O(1) space
 * - compute_quadratic: O(1) time, O(1) space
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// ----------------------------------------------------------------------------
// Function Prototypes (Declarations)
// ----------------------------------------------------------------------------
bool is_prime(long long n);
double compute_power(double base, int exponent);
bool solve_quadratic(double a, double b, double c, double *root1, double *root2);
static void print_section_header(const char *title);

// ----------------------------------------------------------------------------
// Main Driver Function
// ----------------------------------------------------------------------------
int main(void) {
    print_section_header("DAY 26: USER-DEFINED FUNCTIONS & MODULAR ARCHITECTURE");

    // 1. Testing is_prime pure computational function
    printf("[+] Testing Prime Number Detection:\n");
    long long test_numbers[] = {1, 2, 17, 25, 97, 100, 104729};
    size_t num_count = sizeof(test_numbers) / sizeof(test_numbers[0]);

    for (size_t i = 0; i < num_count; ++i) {
        long long val = test_numbers[i];
        printf("    Number: %-8lld -> %s\n", val, is_prime(val) ? "[PRIME]" : "[COMPOSITE / NON-PRIME]");
    }
    printf("\n");

    // 2. Testing integer power computation (handling negative exponents)
    printf("[+] Testing Exponentiation Computation:\n");
    printf("    2.0 ^  8 = %10.4f\n", compute_power(2.0, 8));
    printf("    5.0 ^  3 = %10.4f\n", compute_power(5.0, 3));
    printf("    2.0 ^ -3 = %10.4f\n", compute_power(2.0, -3));
    printf("    9.0 ^  0 = %10.4f\n\n", compute_power(9.0, 0));

    // 3. Testing Quadratic Equation Solver (Multiple Output via Pointers)
    // Equation: 1x^2 - 5x + 6 = 0  => (x - 2)(x - 3) = 0 => roots: 3, 2
    printf("[+] Testing Quadratic Solver (x^2 - 5x + 6 = 0):\n");
    double r1 = 0.0, r2 = 0.0;
    if (solve_quadratic(1.0, -5.0, 6.0, &r1, &r2)) {
        printf("    Roots: Root 1 = %.2f, Root 2 = %.2f\n", r1, r2);
    } else {
        printf("    No real roots exist.\n");
    }

    // Equation with no real roots: 1x^2 + 2x + 5 = 0 (Discriminant < 0)
    printf("[+] Testing Quadratic Solver with Complex Roots (x^2 + 2x + 5 = 0):\n");
    if (!solve_quadratic(1.0, 2.0, 5.0, &r1, &r2)) {
        printf("    Status: [PASS] Correctly detected negative discriminant (no real roots).\n");
    }

    print_section_header("Execution completed successfully.");
    return 0;
}

// ----------------------------------------------------------------------------
// Function Definitions
// ----------------------------------------------------------------------------

/**
 * Helper 1: Formatted visual banner printer (static linkage)
 */
static void print_section_header(const char *title) {
    printf("============================================================\n");
    printf("   %s\n", title);
    printf("============================================================\n\n");
}

/**
 * Helper 2: Prime Number Detector
 * Returns true if n is prime, false otherwise.
 */
bool is_prime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    // Trial division up to sqrt(n) with 6k +/- 1 optimization
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

/**
 * Helper 3: Exponentiation calculator
 */
double compute_power(double base, int exponent) {
    if (exponent == 0) return 1.0;
    if (base == 0.0) return 0.0;

    double result = 1.0;
    long long exp = exponent;
    if (exp < 0) {
        base = 1.0 / base;
        exp = -exp;
    }

    while (exp > 0) {
        if (exp % 2 == 1) {
            result *= base;
        }
        base *= base;
        exp /= 2;
    }
    return result;
}

/**
 * Helper 4: Quadratic Equation Solver
 * Solves a*x^2 + b*x + c = 0 using the quadratic formula: x = (-b +- sqrt(D)) / (2a)
 * Returns false if a == 0 or discriminant < 0.
 */
bool solve_quadratic(double a, double b, double c, double *root1, double *root2) {
    if (a == 0.0 || !root1 || !root2) {
        return false; // Not a quadratic equation or null pointer
    }

    double discriminant = (b * b) - (4.0 * a * c);
    if (discriminant < 0.0) {
        return false; // Complex roots
    }

    double sqrt_d = sqrt(discriminant);
    *root1 = (-b + sqrt_d) / (2.0 * a);
    *root2 = (-b - sqrt_d) / (2.0 * a);
    return true;
}

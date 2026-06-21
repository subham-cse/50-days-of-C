/**
 * ============================================================================
 * Day 15: For Loops & Counted Iteration Mechanics
 * Description : Production demonstration of for loops: Prime sieve generation
 *               (Eratosthenes), vector arithmetic, matrix accumulation, and
 *               multi-variable header manipulation.
 * Compilation : gcc -Wall -Wextra -std=c99 15_For_Loop_code.c -o 15_For_Loop_code
 * Execution   : ./15_For_Loop_code
 * 
 * Complexity  : Sieve of Eratosthenes: O(N log log N)
 *               Vector Accumulation: O(N)
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SIEVE_LIMIT 100

/**
 * @brief Computes prime numbers up to limit using Sieve of Eratosthenes with for-loops.
 * @param limit Upper bound inclusive (<= MAX_SIEVE_LIMIT).
 * @param is_prime Output boolean array of size (limit + 1).
 */
void sieve_of_eratosthenes(size_t limit, bool is_prime[]) {
    /* Initialize all numbers as potentially prime */
    for (size_t i = 0; i <= limit; ++i) {
        is_prime[i] = true;
    }
    is_prime[0] = false;
    is_prime[1] = false;

    /* Primary Sieve Loop */
    for (size_t p = 2; p * p <= limit; ++p) {
        if (is_prime[p]) {
            /* Mark multiples of p starting from p*p */
            for (size_t multiple = p * p; multiple <= limit; multiple += p) {
                is_prime[multiple] = false;
            }
        }
    }
}

/**
 * @brief Reverses an array in-place using two loop pointers in a single for header.
 * @param arr Array of integers.
 * @param length Total number of elements.
 */
void reverse_array_in_place(int arr[], size_t length) {
    if (length <= 1) return;

    /* Multi-variable initialization and step in for-header */
    for (size_t left = 0, right = length - 1; left < right; ++left, --right) {
        int temp = arr[left];
        arr[left] = arr[right];
        arr[right] = temp;
    }
}

/**
 * @brief Computes inner dot product of two vectors of identical dimensions.
 * @param vec_a Pointer to vector A elements.
 * @param vec_b Pointer to vector B elements.
 * @param dim Dimensionality of vectors.
 * @return Scalar dot product result.
 */
double compute_dot_product(const double vec_a[], const double vec_b[], size_t dim) {
    double sum = 0.0;
    for (size_t i = 0; i < dim; ++i) {
        sum += vec_a[i] * vec_b[i];
    }
    return sum;
}

int main(void) {
    printf("===================================================================\n");
    printf("               DAY 15: FOR LOOP ITERATION SYSTEMS                  \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: Sieve of Eratosthenes */
    printf("[TEST SUITE 1: Prime Sieve (2 to %d)]\n", MAX_SIEVE_LIMIT);
    bool is_prime[MAX_SIEVE_LIMIT + 1];
    sieve_of_eratosthenes(MAX_SIEVE_LIMIT, is_prime);

    size_t prime_count = 0;
    for (size_t i = 2; i <= MAX_SIEVE_LIMIT; ++i) {
        if (is_prime[i]) {
            printf("%3zu ", i);
            prime_count++;
            if (prime_count % 10 == 0) printf("\n");
        }
    }
    printf("\nTotal Primes Found: %zu\n\n", prime_count);

    /* Test Suite 2: Multi-Variable For Loop Array Reversal */
    printf("[TEST SUITE 2: Dual-Index In-Place Array Reversal]\n");
    int sample_data[] = {10, 20, 30, 40, 50, 60, 70, 80};
    size_t data_len = sizeof(sample_data) / sizeof(sample_data[0]);

    printf("Original: ");
    for (size_t i = 0; i < data_len; ++i) printf("%d ", sample_data[i]);
    printf("\n");

    reverse_array_in_place(sample_data, data_len);

    printf("Reversed: ");
    for (size_t i = 0; i < data_len; ++i) printf("%d ", sample_data[i]);
    printf("\n\n");

    /* Test Suite 3: Vector Dot Product Accumulation */
    printf("[TEST SUITE 3: Vector Linear Algebra Dot Product]\n");
    double u[] = {1.5, 2.0, -3.5, 4.0};
    double v[] = {0.5, 4.0,  2.0, 1.5};
    size_t dim = sizeof(u) / sizeof(u[0]);

    double dot_res = compute_dot_product(u, v, dim);
    printf("Dot Product result: %.4f\n", dot_res);

    printf("===================================================================\n");
    return 0;
}

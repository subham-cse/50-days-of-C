/**
 * ============================================================================
 * Day 16: Do-While Loop & Exit-Controlled Iteration
 * Description : Production demonstration of post-test do-while loops:
 *               Exponential backoff retry simulator, numerical fixed-point
 *               square root (Newton-Raphson), and macro safety encapsulation.
 * Compilation : gcc -Wall -Wextra -std=c99 16_Do_While_Loop_code.c -o 16_Do_While_Loop_code
 * Execution   : ./16_Do_While_Loop_code
 * 
 * Complexity  : Newton-Raphson: O(log(Precision)) quadratic convergence
 *               Retry Backoff: O(MaxAttempts)
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/* Production idiom: Safe multi-statement macro */
#define SAFE_SWAP(T, a, b) \
    do {                   \
        T _tmp = (a);      \
        (a) = (b);         \
        (b) = _tmp;        \
    } while (0)

typedef enum {
    TRANSACTION_SUCCESS = 0,
    TRANSACTION_TIMEOUT,
    TRANSACTION_RETRYING
} TxStatus;

/**
 * @brief Simulates an idempotent network transaction with exponential backoff.
 * @param max_retries Maximum number of attempts allowed.
 * @param failure_count Simulates hardware failures before succeeding.
 * @return Final transaction status.
 */
TxStatus simulate_network_transaction_with_backoff(uint32_t max_retries, uint32_t failure_count) {
    uint32_t attempt = 0;
    uint32_t delay_ms = 50;
    TxStatus status;

    do {
        attempt++;
        printf("  [Attempt %u/%u] Transmitting packet (Current Backoff Delay: %u ms)... ",
               attempt, max_retries, delay_ms);

        if (attempt <= failure_count) {
            printf("FAILED (Packet Dropped)\n");
            status = TRANSACTION_RETRYING;
            delay_ms *= 2; /* Exponential multiplier */
        } else {
            printf("SUCCESS (ACK Received)\n");
            status = TRANSACTION_SUCCESS;
            break;
        }
    } while (attempt < max_retries);

    if (status != TRANSACTION_SUCCESS) {
        status = TRANSACTION_TIMEOUT;
    }

    return status;
}

/**
 * @brief Computes square root using Newton-Raphson method via do-while convergence loop.
 * @param target Non-negative number to evaluate.
 * @param epsilon Allowed convergence tolerance.
 * @return Approximate square root.
 */
double compute_sqrt_newton_raphson(double target, double epsilon) {
    if (target <= 0.0) return 0.0;

    double guess = target / 2.0;
    if (guess == 0.0) guess = 1.0;

    double next_guess;
    double diff;

    do {
        /* Newton's formula: x_{n+1} = 0.5 * (x_n + target / x_n) */
        next_guess = 0.5 * (guess + target / guess);
        diff = next_guess - guess;
        if (diff < 0.0) diff = -diff; /* Absolute value without math.h fabs */

        guess = next_guess;
    } while (diff > epsilon);

    return guess;
}

int main(void) {
    printf("===================================================================\n");
    printf("            DAY 16: DO-WHILE POST-TEST LOOP ENGINE                 \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: Newton-Raphson Convergence */
    printf("[TEST SUITE 1: Newton-Raphson Square Root Approximation]\n");
    double test_values[] = {2.0, 9.0, 144.0, 2048.0, 65536.0};
    size_t num_vals = sizeof(test_values) / sizeof(test_values[0]);
    double tolerance = 1e-9;

    for (size_t i = 0; i < num_vals; ++i) {
        double val = test_values[i];
        double approx = compute_sqrt_newton_raphson(val, tolerance);
        printf("Target: %9.1f | Computed Sqrt: %14.8f\n", val, approx);
    }

    /* Test Suite 2: Network Exponential Backoff */
    printf("\n[TEST SUITE 2: Exponential Backoff Retry Loop]\n");
    printf("Scenario A: Resolves on 3rd attempt (Max 5):\n");
    TxStatus res_a = simulate_network_transaction_with_backoff(5, 2);
    printf("Final Result: %s\n\n", (res_a == TRANSACTION_SUCCESS) ? "COMMITTED" : "ABORTED");

    printf("Scenario B: Exceeds Max Retries (Max 3, Failures 5):\n");
    TxStatus res_b = simulate_network_transaction_with_backoff(3, 5);
    printf("Final Result: %s\n\n", (res_b == TRANSACTION_SUCCESS) ? "COMMITTED" : "ABORTED");

    /* Test Suite 3: Safe Macro Demonstration */
    printf("[TEST SUITE 3: Multi-Statement Macro with do { ... } while(0)]\n");
    int x = 100, y = 999;
    printf("Before Swap: x = %d, y = %d\n", x, y);
    if (x < y)
        SAFE_SWAP(int, x, y);
    else
        printf("No swap needed\n");
    printf("After Swap : x = %d, y = %d\n", x, y);

    printf("===================================================================\n");
    return 0;
}

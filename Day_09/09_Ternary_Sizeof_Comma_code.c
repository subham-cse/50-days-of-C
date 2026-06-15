/**
 * ============================================================================
 * DAY 09: Ternary Conditional, Sizeof, and Comma Operators
 * ============================================================================
 * Description:
 *   Demonstrates the ternary conditional operator (?:) for branchless clamp logic,
 *   compile-time sizeof mechanics with side-effect non-evaluation verification,
 *   array decay sizing contrasts, and comma operator sequencing.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 09_Ternary_Sizeof_Comma_code.c -o day09_special_ops
 *
 * Execution:
 *   ./day09_special_ops
 *
 * Complexity Metrics:
 *   - Time Complexity: O(1) for sizing and clamp evaluations; O(N) for comma loop
 *   - Space Complexity: O(1) auxiliary frame stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* ============================================================================
 * 1. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * clamp_integer_ternary:
 *   Constrains a value within [min_val, max_val] using nested ternary expressions.
 *
 * Parameters:
 *   val     - Input integer value.
 *   min_val - Lower bound limit.
 *   max_val - Upper bound limit.
 *
 * Returns:
 *   int32_t - Clamped value within valid bounds.
 */
int32_t clamp_integer_ternary(int32_t val, int32_t min_val, int32_t max_val)
{
    return (val < min_val) ? min_val : ((val > max_val) ? max_val : val);
}

/**
 * verify_sizeof_side_effects:
 *   Proves that expressions within sizeof are strictly unevaluated at runtime
 *   by monitoring an instrumented counter before and after sizeof execution.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   bool - True if counter remained unmodified (proving compile-time evaluation).
 */
bool verify_sizeof_side_effects(void)
{
    int32_t tracked_counter = 42;

    printf("=== Sizeof Non-Evaluation Proof ===\n");
    printf("  Initial tracked_counter value: %d\n", tracked_counter);

    /* The expression '++tracked_counter' inside sizeof is NEVER executed at runtime */
    size_t result_size = sizeof(++tracked_counter);

    printf("  Evaluated sizeof(++tracked_counter) : %zu bytes\n", result_size);
    printf("  Value of tracked_counter afterwards : %d\n", tracked_counter);

    bool is_unevaluated = (tracked_counter == 42);
    printf("  Side-Effect Verification: %s\n\n",
           is_unevaluated ? "CONFIRMED (Zero runtime execution)" : "FAILED (Unexpected side-effect)");

    return is_unevaluated;
}

/**
 * demonstrate_comma_operator:
 *   Illustrates sequential evaluation and discarded values using the comma operator
 *   inside compound assignment and loop update expressions.
 *
 * Parameters:
 *   iterations - Loop upper bound.
 *
 * Returns:
 *   int32_t - Final accumulated sum.
 */
int32_t demonstrate_comma_operator(int iterations)
{
    int32_t sum = 0;
    int32_t step_counter = 0;

    printf("=== Comma Operator Sequencing Demo ===\n");

    /* Comma operator in loop initializer and post-iteration expressions */
    for (int i = 0, j = iterations; i <= iterations; ++i, --j, ++step_counter) {
        sum += (i * 2);
        printf("  [Step %2d] i = %2d | j = %2d | Current Sum = %3d\n", step_counter, i, j, sum);
    }

    /* Comma operator in standalone expression */
    int32_t comma_expr_val = (step_counter += 5, step_counter * 10);
    printf("  Evaluated `(step_counter += 5, step_counter * 10)` => %d (Final step_counter: %d)\n\n",
           comma_expr_val, step_counter);

    return sum;
}

/* ============================================================================
 * 2. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 09: Ternary Conditional, Sizeof, and Comma Operators\n");
    printf("==============================================================\n\n");

    /* 1. Ternary Clamping Evaluation */
    printf("--- 1. Nested Ternary Clamp Logic ---\n");
    int32_t test_inputs[] = {-25, 0, 50, 100, 150};
    for (size_t i = 0; i < ARRAY_SIZE(test_inputs); ++i) {
        int32_t v = test_inputs[i];
        int32_t clamped = clamp_integer_ternary(v, 0, 100);
        printf("  Input: %4d -> Clamped [0, 100]: %3d\n", v, clamped);
    }
    printf("\n");

    /* 2. Sizeof Side-Effect Verification */
    verify_sizeof_side_effects();

    /* 3. Array Sizing vs Pointer Decay */
    printf("--- 3. Array Size Macro vs. Struct Sizing ---\n");
    double sensor_readings[16];
    printf("  sizeof(sensor_readings)               : %zu bytes\n", sizeof(sensor_readings));
    printf("  sizeof(sensor_readings[0])            : %zu bytes\n", sizeof(sensor_readings[0]));
    printf("  ARRAY_SIZE(sensor_readings)           : %zu elements\n\n", ARRAY_SIZE(sensor_readings));

    /* 4. Comma Operator Demonstration */
    demonstrate_comma_operator(4);

    printf("==============================================================\n");
    return EXIT_SUCCESS;
}

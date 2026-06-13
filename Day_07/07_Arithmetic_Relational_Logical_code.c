/**
 * ============================================================================
 * DAY 07: Arithmetic, Relational, and Logical Operators
 * ============================================================================
 * Description:
 *   Demonstrates arithmetic rules (truncated division, modulo sign invariants),
 *   prefix vs. postfix side effects, relational assertions, and short-circuit
 *   evaluation mechanics with instrumented side-effect probes.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 07_Arithmetic_Relational_Logical_code.c -o day07_operators
 *
 * Execution:
 *   ./day07_operators
 *
 * Complexity Metrics:
 *   - Time Complexity: O(1) constant-time arithmetic logic evaluations
 *   - Space Complexity: O(1) auxiliary frame memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 1. INSTRUMENTED SIDE-EFFECT PROBES
 * ============================================================================
 */

static uint32_t g_probe_calls = 0;

/**
 * probe_function:
 *   Helper function with intentional side-effects used to demonstrate whether
 *   an operand was evaluated or skipped due to short-circuit logic.
 *
 * Parameters:
 *   id         - Unique identifier for the probe site.
 *   return_val - Boolean value to return.
 *
 * Returns:
 *   bool - The supplied return_val.
 */
static bool probe_function(int id, bool return_val)
{
    g_probe_calls++;
    printf("    [Probe #%d Invoked] Returning %s (Global Probe Calls: %u)\n",
           id, return_val ? "TRUE" : "FALSE", g_probe_calls);
    return return_val;
}

/* ============================================================================
 * 2. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * verify_modulo_invariants:
 *   Tests the ISO C99 truncated integer division and modulo sign invariant:
 *   `(a / b) * b + (a % b) == a` across negative and positive combinations.
 *
 * Parameters:
 *   a - Dividend.
 *   b - Divisor (must not be zero).
 *
 * Returns:
 *   bool - True if mathematical identity holds.
 */
bool verify_modulo_invariants(int32_t a, int32_t b)
{
    if (b == 0) {
        fprintf(stderr, "[ERROR] Division by zero requested!\n");
        return false;
    }

    int32_t quotient = a / b;
    int32_t remainder = a % b;
    int32_t reconstructed = (quotient * b) + remainder;

    printf("  Division Test: (%3d / %3d) = %3d | (%3d %% %3d) = %3d | Invariant Match: %s\n",
           a, b, quotient, a, b, remainder, (reconstructed == a) ? "PASS" : "FAIL");

    return (reconstructed == a);
}

/**
 * demonstrate_short_circuit:
 *   Demonstrates short-circuit sequence point guarantees for logical AND/OR.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 */
void demonstrate_short_circuit(void)
{
    printf("\n=== Short-Circuit Evaluation Analysis ===\n");

    /* Test Case A: Logical AND with False Left-Hand Operand */
    g_probe_calls = 0;
    printf("  Evaluating: probe(1, false) && probe(2, true)\n");
    if (probe_function(1, false) && probe_function(2, true)) {
        printf("    Branch: TRUE\n");
    } else {
        printf("    Branch: FALSE (Probe #2 correctly SKIPPED via short-circuit)\n");
    }

    /* Test Case B: Logical OR with True Left-Hand Operand */
    printf("\n  Evaluating: probe(3, true) || probe(4, false)\n");
    if (probe_function(3, true) || probe_function(4, false)) {
        printf("    Branch: TRUE (Probe #4 correctly SKIPPED via short-circuit)\n");
    } else {
        printf("    Branch: FALSE\n");
    }
    printf("=========================================\n\n");
}

/* ============================================================================
 * 3. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 07: Arithmetic, Relational, and Logical Operators\n");
    printf("==============================================================\n\n");

    /* 1. Arithmetic Modulo Sign Invariant Testing */
    printf("--- 1. Integer Division & Modulo Sign Invariants ---\n");
    verify_modulo_invariants(17, 5);
    verify_modulo_invariants(-17, 5);
    verify_modulo_invariants(17, -5);
    verify_modulo_invariants(-17, -5);

    /* 2. Prefix vs Postfix Evaluation Mechanics */
    printf("\n--- 2. Prefix vs Postfix Increment Mechanics ---\n");
    int32_t val = 10;
    printf("  Initial val         = %d\n", val);
    int32_t postfix_res = val++;
    printf("  val++ Expression    = %d (Returns old value, val becomes %d)\n", postfix_res, val);
    int32_t prefix_res = ++val;
    printf("  ++val Expression    = %d (Returns incremented value, val becomes %d)\n", prefix_res, val);

    /* 3. Short-Circuit Logic Evaluation */
    demonstrate_short_circuit();

    return EXIT_SUCCESS;
}

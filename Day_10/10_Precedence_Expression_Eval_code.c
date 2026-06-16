/**
 * ============================================================================
 * DAY 10: Operator Precedence, Associativity, and Order of Evaluation
 * ============================================================================
 * Description:
 *   Demonstrates operator precedence hierarchies, associativity pitfalls,
 *   pointer dereference vs increment ambiguities (*p++ vs (*p)++), bitwise
 *   vs relational precedence traps, and subexpression order of evaluation probes.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 10_Precedence_Expression_Eval_code.c -o day10_precedence
 *
 * Execution:
 *   ./day10_precedence
 *
 * Complexity Metrics:
 *   - Time Complexity: O(1) constant time expression evaluations
 *   - Space Complexity: O(1) auxiliary frame stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 1. INSTRUMENTED ORDER OF EVALUATION PROBES
 * ============================================================================
 */

static int g_eval_sequence[3];
static size_t g_eval_idx = 0;

static int eval_subexpr(int id, int value)
{
    if (g_eval_idx < 3) {
        g_eval_sequence[g_eval_idx++] = id;
    }
    printf("    [Subexpression Probe #%d Evaluated] Value: %d\n", id, value);
    return value;
}

/* ============================================================================
 * 2. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * test_precedence_traps:
 *   Demonstrates common subtle bugs arising from misunderstood operator precedence.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 */
void test_precedence_traps(void)
{
    printf("=== Common Precedence Pitfalls Demonstration ===\n");

    /* Trap 1: Bitwise AND vs Equality (== binds higher than &) */
    uint32_t flags = 0x01;
    uint32_t mask = 0x01;

    bool unparenthesized = flags & mask == 0;      /* Evaluates as flags & (mask == 0) -> 0x01 & 0 -> 0 */
    bool parenthesized   = (flags & mask) == 0;    /* Evaluates as (0x01 & 0x01) == 0 -> 1 == 0 -> 0 */

    printf("  [Trap 1: Bitwise & vs ==]\n");
    printf("    Expression `flags & mask == 0`   : %s\n", unparenthesized ? "TRUE" : "FALSE (0)");
    printf("    Expression `(flags & mask) == 0` : %s\n\n", parenthesized ? "TRUE" : "FALSE (0)");

    /* Trap 2: Shift vs Addition (+ binds higher than <<) */
    int val = 1;
    int shift_trap     = val << 2 + 1;             /* Evaluates as 1 << (2 + 1) = 1 << 3 = 8 */
    int shift_intended = (val << 2) + 1;           /* Evaluates as (1 << 2) + 1 = 4 + 1 = 5 */

    printf("  [Trap 2: Shift << vs Addition +]\n");
    printf("    Expression `val << 2 + 1`        : %d (Shifted by 3!)\n", shift_trap);
    printf("    Expression `(val << 2) + 1`      : %d (Shifted by 2, then added 1)\n\n", shift_intended);

    /* Trap 3: Pointer Dereference vs Post-Increment */
    int data[3] = {10, 20, 30};
    int *p1 = data;

    printf("  [Trap 3: *p++ vs (*p)++]\n");
    int deref_postinc = *p1++;                     /* Fetches *p1 (10), then advances p1 to &data[1] */
    printf("    *p1++ returned                   : %d (p1 now points to value %d)\n", deref_postinc, *p1);

    int *p2 = data;
    int inc_deref_val = (*p2)++;                   /* Fetches *p2 (10), increments data[0] to 11 */
    printf("    (*p2)++ returned                 : %d (data[0] incremented to %d)\n", inc_deref_val, data[0]);
    printf("================================================\n\n");
}

/**
 * demonstrate_order_of_evaluation:
 *   Illustrates compiler autonomy in evaluating operands of binary arithmetic expressions.
 *
 * Parameters:
 *   void
 *
 * Returns:
 *   void
 */
void demonstrate_order_of_evaluation(void)
{
    g_eval_idx = 0;
    printf("=== Expression Order of Evaluation Tracking ===\n");
    printf("  Evaluating expression: f(1, 10) + g(2, 20) * h(3, 30)\n");

    int final_result = eval_subexpr(1, 10) + (eval_subexpr(2, 20) * eval_subexpr(3, 30));

    printf("  Chronological Probe Order Recorded : [%d -> %d -> %d]\n",
           g_eval_sequence[0], g_eval_sequence[1], g_eval_sequence[2]);
    printf("  Final Arithmetic Result            : %d\n", final_result);
    printf("  [Note] Precedence guarantees * evaluates before +, but operand call order is unspecified.\n");
    printf("================================================\n\n");
}

/* ============================================================================
 * 3. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 10: Operator Precedence, Associativity & Evaluation Order\n");
    printf("==============================================================\n\n");

    /* 1. Test Precedence Pitfalls */
    test_precedence_traps();

    /* 2. Test Order of Evaluation */
    demonstrate_order_of_evaluation();

    return EXIT_SUCCESS;
}

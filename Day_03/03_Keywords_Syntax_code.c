/**
 * ============================================================================
 * DAY 03: Keywords, Storage Classes, Type Qualifiers, and Syntax Structures
 * ============================================================================
 * Description:
 *   Demonstrates practical applications of C keywords: storage duration classes
 *   (auto, static, register, extern), type qualifiers (const, volatile, restrict),
 *   and structured flow control blocks.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 03_Keywords_Syntax_code.c -o day03_keywords
 *
 * Execution:
 *   ./day03_keywords
 *
 * Complexity Metrics:
 *   - Time Complexity: O(N) where N is array length processed
 *   - Space Complexity: O(1) auxiliary frame memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * 1. FILE-SCOPE DECLARATIONS & CONSTANTS
 * ============================================================================
 */

/* Storage Class: static (internal linkage), Qualifier: const (read-only) */
static const char g_module_tag[] = "KEYWORD_SYNTAX_MODULE";

/* Demonstration enumeration using enum and typedef */
typedef enum SystemStatus {
    STATUS_IDLE       = 0,
    STATUS_PROCESSING = 1,
    STATUS_COMPLETED  = 2,
    STATUS_ERROR      = -1
} SystemStatus_t;

/* ============================================================================
 * 2. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * demonstrate_storage_classes:
 *   Contrasts automatic, static, and register storage durations inside
 *   block scope across repeated function invocations.
 *
 * Parameters:
 *   invocation_id - Current call sequence number.
 *
 * Returns:
 *   int32_t - The updated persistent static counter value.
 */
int32_t demonstrate_storage_classes(int invocation_id)
{
    /* Automatic variable: reallocated and reinitialized on stack every call */
    auto int auto_counter = 0;

    /* Static block variable: allocated in .data/.bss, persists across calls */
    static int32_t static_persistent_counter = 100;

    /* Register hint: request fast CPU register placement */
    register int step_increment = 5;

    auto_counter += step_increment;
    static_persistent_counter += step_increment;

    printf("  [Call #%d] auto_counter: %d (reset) | static_persistent_counter: %d (accumulated)\n",
           invocation_id, auto_counter, static_persistent_counter);

    return static_persistent_counter;
}

/**
 * vector_scaled_add_restrict:
 *   Demonstrates C99 'restrict' pointer qualifier informing the compiler that
 *   pointers 'dest', 'src_a', and 'src_b' do not alias overlapping memory.
 *
 * Parameters:
 *   dest   - Destination memory buffer (restricted pointer).
 *   src_a  - First input buffer (const restricted pointer).
 *   src_b  - Second input buffer (const restricted pointer).
 *   scale  - Multiplier scalar.
 *   length - Number of elements to process.
 *
 * Returns:
 *   bool - True if processing succeeded, False on invalid null arguments.
 */
bool vector_scaled_add_restrict(int32_t * restrict dest,
                                const int32_t * restrict src_a,
                                const int32_t * restrict src_b,
                                int32_t scale,
                                size_t length)
{
    if (dest == NULL || src_a == NULL || src_b == NULL || length == 0) {
        return false;
    }

    for (size_t i = 0; i < length; ++i) {
        dest[i] = (src_a[i] * scale) + src_b[i];
    }

    return true;
}

/**
 * evaluate_system_state:
 *   Demonstrates selection flow control keywords: switch, case, default, break.
 *
 * Parameters:
 *   status - Current SystemStatus_t code.
 *
 * Returns:
 *   const char * - Human-readable diagnostic description string.
 */
const char *evaluate_system_state(SystemStatus_t status)
{
    switch (status) {
        case STATUS_IDLE:
            return "System is IDLE and awaiting instructions.";
        case STATUS_PROCESSING:
            return "System is currently PROCESSING workloads.";
        case STATUS_COMPLETED:
            return "System has COMPLETED all assigned tasks.";
        case STATUS_ERROR:
            return "System encountered a CRITICAL ERROR state.";
        default:
            return "UNKNOWN or UNDEFINED status code received.";
    }
}

/* ============================================================================
 * 3. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 03: Keywords, Storage Classes, and Type Qualifiers\n");
    printf(" Module Tag: %s\n", g_module_tag);
    printf("==============================================================\n\n");

    /* 1. Demonstrate Storage Duration Mechanics */
    printf("--- 1. Automatic vs Static Storage Duration ---\n");
    for (int i = 1; i <= 3; ++i) {
        demonstrate_storage_classes(i);
    }
    printf("\n");

    /* 2. Demonstrate C99 'restrict' and Vector Processing */
    printf("--- 2. C99 restrict Qualifier Vector Processing ---\n");
    #define VEC_SIZE 5
    int32_t vec_a[VEC_SIZE] = {1, 2, 3, 4, 5};
    int32_t vec_b[VEC_SIZE] = {10, 20, 30, 40, 50};
    int32_t vec_out[VEC_SIZE] = {0};
    const int32_t scale_factor = 3;

    if (vector_scaled_add_restrict(vec_out, vec_a, vec_b, scale_factor, VEC_SIZE)) {
        printf("Computed result array (vec_a * %d + vec_b):\n  ", scale_factor);
        for (size_t i = 0; i < VEC_SIZE; ++i) {
            printf("[%zu]: %d ", i, vec_out[i]);
        }
        printf("\n\n");
    }

    /* 3. Demonstrate Enum State Machine Selection */
    printf("--- 3. Enumeration and Switch/Case Evaluation ---\n");
    SystemStatus_t test_states[] = {STATUS_IDLE, STATUS_PROCESSING, STATUS_COMPLETED, STATUS_ERROR};
    for (size_t i = 0; i < sizeof(test_states) / sizeof(test_states[0]); ++i) {
        printf(" State Code (%2d) => %s\n", test_states[i], evaluate_system_state(test_states[i]));
    }
    printf("==============================================================\n");

    return EXIT_SUCCESS;
}

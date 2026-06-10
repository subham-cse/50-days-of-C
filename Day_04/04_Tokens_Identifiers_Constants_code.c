/**
 * ============================================================================
 * DAY 04: Tokens, Identifiers, Numerical Constants, and String Literals
 * ============================================================================
 * Description:
 *   Demonstrates lexical tokens, radix representations (decimal, octal, hex),
 *   type suffixes (ULL, F, L), string literal memory placement versus mutable
 *   stack arrays, and character constant evaluation.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 04_Tokens_Identifiers_Constants_code.c -o day04_tokens
 *
 * Execution:
 *   ./day04_tokens
 *
 * Complexity Metrics:
 *   - Time Complexity: O(N) where N is character count in test strings
 *   - Space Complexity: O(1) auxiliary frame stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 1. FILE-SCOPE CONSTANTS & MACROS
 * ============================================================================
 */

/* Multi-radix representations of the exact same integer value (255) */
static const int32_t CONST_DECIMAL = 255;
static const int32_t CONST_OCTAL   = 0377;
static const int32_t CONST_HEX     = 0x00FF;

/* Floating point constants with suffixes */
static const float       FLOAT_SINGLE    = 3.14159265f;
static const double      FLOAT_DOUBLE    = 3.141592653589793;
static const long double FLOAT_EXTENDED  = 3.14159265358979323846L;

/* ============================================================================
 * 2. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * verify_radix_equivalence:
 *   Demonstrates how diverse lexical tokens represent equivalent bit patterns
 *   in binary storage.
 *
 * Parameters:
 *   dec_val - Decimal integer representation.
 *   oct_val - Octal integer representation.
 *   hex_val - Hexadecimal integer representation.
 *
 * Returns:
 *   bool - True if all three expressions evaluate to identical numeric values.
 */
bool verify_radix_equivalence(int32_t dec_val, int32_t oct_val, int32_t hex_val)
{
    printf("=== Radix Equivalence Analysis ===\n");
    printf("  Decimal Token (255)  : %d (0x%04X)\n", dec_val, (unsigned int)dec_val);
    printf("  Octal Token   (0377) : %d (0x%04X)\n", oct_val, (unsigned int)oct_val);
    printf("  Hex Token     (0x00FF): %d (0x%04X)\n", hex_val, (unsigned int)hex_val);

    bool is_identical = (dec_val == oct_val) && (oct_val == hex_val);
    printf("  Radix Identity Verification: %s\n\n", is_identical ? "PASSED (100%% Match)" : "FAILED");

    return is_identical;
}

/**
 * compare_string_storage:
 *   Contrasts the mutable stack character array against the read-only
 *   string literal residing in .rodata.
 *
 * Parameters:
 *   rodata_ptr  - Pointer to read-only string literal in .rodata.
 *   stack_array - Pointer to stack-allocated mutable character buffer.
 *   buffer_len  - Total allocated length of stack buffer.
 *
 * Returns:
 *   void
 */
void compare_string_storage(const char *rodata_ptr, char *stack_array, size_t buffer_len)
{
    printf("=== String Literal vs. Mutable Array Memory Inspection ===\n");
    printf("  Read-Only String Literal Location (.rodata) : %p -> \"%s\"\n",
           (const void *)rodata_ptr, rodata_ptr);
    printf("  Mutable Stack Array Location (Stack Frame)  : %p -> \"%s\"\n",
           (void *)stack_array, stack_array);

    /* Safe mutation of the stack-allocated character array */
    if (buffer_len > 0) {
        stack_array[0] = 'X';
        printf("  Stack array mutated index [0] to 'X'        : \"%s\"\n", stack_array);
    }

    printf("  [Warning] Attempting to mutate rodata_ptr[0] would trigger a SIGSEGV / Protection Fault!\n\n");
}

/* ============================================================================
 * 3. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 04: Tokens, Numerical Literals, and Storage Semantics\n");
    printf("==============================================================\n\n");

    /* Helper Function 1: Radix equivalence testing */
    verify_radix_equivalence(CONST_DECIMAL, CONST_OCTAL, CONST_HEX);

    /* String Literals and Storage */
    const char *rodata_string = "Embedded Immutable String Literal";
    char stack_buffer[] = "Editable Stack Buffer";

    /* Helper Function 2: String storage analysis */
    compare_string_storage(rodata_string, stack_buffer, sizeof(stack_buffer));

    /* Floating Point Suffix Diagnostic */
    printf("=== Floating-Point Literal Evaluation ===\n");
    printf("  sizeof(float) single   [3.14159f]  : %zu bytes (Val: %.7f)\n", 
           sizeof(FLOAT_SINGLE), (double)FLOAT_SINGLE);
    printf("  sizeof(double) default [3.14159]   : %zu bytes (Val: %.15f)\n", 
           sizeof(FLOAT_DOUBLE), FLOAT_DOUBLE);
    printf("  sizeof(long double)    [3.14159L]  : %zu bytes (Val: %.18Lf)\n\n", 
           sizeof(FLOAT_EXTENDED), FLOAT_EXTENDED);

    /* Character Constant Sizing Demonstration */
    printf("=== Character Constant Sizing in C vs C++ ===\n");
    printf("  sizeof('A') in ISO C               : %zu bytes (Evaluates to int)\n", sizeof('A'));
    printf("  sizeof((char)'A') explicitly cast  : %zu byte\n", sizeof((char)'A'));
    printf("==============================================================\n");

    return EXIT_SUCCESS;
}

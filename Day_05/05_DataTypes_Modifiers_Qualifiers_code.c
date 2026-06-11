/**
 * ============================================================================
 * DAY 05: Data Types, Width Modifiers, Qualifiers, and Binary Encodings
 * ============================================================================
 * Description:
 *   Demonstrates primitive and fixed-width data types, memory byte dumps,
 *   Two's Complement bit patterns, IEEE-754 float layouts, integer promotions,
 *   and signed vs. unsigned comparison hazards.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 05_DataTypes_Modifiers_Qualifiers_code.c -o day05_datatypes
 *
 * Execution:
 *   ./day05_datatypes
 *
 * Complexity Metrics:
 *   - Time Complexity: O(B) where B is the byte length of data types inspected
 *   - Space Complexity: O(1) auxiliary frame memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

/* ============================================================================
 * 1. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * dump_memory_bytes:
 *   Inspects and prints the raw binary byte layout of any memory object
 *   in hexadecimal format, illustrating endianness and bit storage.
 *
 * Parameters:
 *   ptr       - Pointer to memory block to inspect.
 *   num_bytes - Number of consecutive bytes to display.
 *   label     - Descriptive name of the data object.
 *
 * Returns:
 *   void
 */
void dump_memory_bytes(const void *ptr, size_t num_bytes, const char *label)
{
    const uint8_t *byte_ptr = (const uint8_t *)ptr;

    printf("  %-24s [%2zu bytes] @ %p: ", label, num_bytes, ptr);
    for (size_t i = 0; i < num_bytes; ++i) {
        printf("%02X ", byte_ptr[i]);
    }
    printf("\n");
}

/**
 * demonstrate_signed_unsigned_hazard:
 *   Illustrates the critical silent bug caused by implicit conversion during
 *   signed vs unsigned comparisons (Usual Arithmetic Conversions).
 *
 * Parameters:
 *   signed_val   - Negative signed integer (e.g. -5).
 *   unsigned_val - Positive unsigned integer (e.g. 10).
 *
 * Returns:
 *   bool - True if standard comparison matched mathematical intuition.
 */
bool demonstrate_signed_unsigned_hazard(int32_t signed_val, uint32_t unsigned_val)
{
    printf("=== Signed vs. Unsigned Comparison Hazard Demo ===\n");
    printf("  Signed Value (int32_t)   : %d (Hex: 0x%08X)\n", signed_val, (uint32_t)signed_val);
    printf("  Unsigned Value (uint32_t): %u (Hex: 0x%08X)\n", unsigned_val, unsigned_val);

    /* Direct comparison triggers implicit conversion of signed_val to uint32_t */
    bool naive_check = (signed_val < (int32_t)unsigned_val);
    bool direct_hazard_check = (signed_val < unsigned_val); /* Triggers compiler warning with -Wsign-compare */

    printf("  Mathematically: (%d < %u) is TRUE\n", signed_val, unsigned_val);
    printf("  Direct C Evaluation `(signed_val < unsigned_val)`: %s\n",
           direct_hazard_check ? "TRUE (1)" : "FALSE (0) [UNEXPECTED TRAP!]");
    printf("  Safe Explicit Cast Check: %s\n\n",
           naive_check ? "TRUE (1) [CORRECT]" : "FALSE (0)");

    return direct_hazard_check;
}

/* ============================================================================
 * 2. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 05: Data Types, Fixed-Width Representation & Encodings\n");
    printf("==============================================================\n\n");

    /* 1. Sizing and Limits Matrix */
    printf("--- 1. Fixed-Width & Standard Integer Sizes ---\n");
    printf("  sizeof(char)      : %zu byte  (Range: %d to %d)\n", sizeof(char), CHAR_MIN, CHAR_MAX);
    printf("  sizeof(short)     : %zu bytes (Range: %d to %d)\n", sizeof(short), SHRT_MIN, SHRT_MAX);
    printf("  sizeof(int)       : %zu bytes (Range: %d to %d)\n", sizeof(int), INT_MIN, INT_MAX);
    printf("  sizeof(long)      : %zu bytes (Platform-dependent)\n", sizeof(long));
    printf("  sizeof(long long) : %zu bytes (Range: %lld to %lld)\n", sizeof(long long), LLONG_MIN, LLONG_MAX);
    printf("  sizeof(size_t)    : %zu bytes\n", sizeof(size_t));
    printf("  sizeof(void *)    : %zu bytes (Pointer width)\n\n", sizeof(void *));

    /* 2. Raw Byte Dump: Two's Complement and IEEE 754 */
    printf("--- 2. Raw Memory Endianness & Byte Dumps ---\n");
    int32_t pos_int = 42;
    int32_t neg_int = -42;
    float single_float = 3.14159f;
    double double_float = 3.141592653589793;

    dump_memory_bytes(&pos_int, sizeof(pos_int), "int32_t (+42)");
    dump_memory_bytes(&neg_int, sizeof(neg_int), "int32_t (-42 Two's Comp)");
    dump_memory_bytes(&single_float, sizeof(single_float), "float (IEEE 754 32-bit)");
    dump_memory_bytes(&double_float, sizeof(double_float), "double (IEEE 754 64-bit)");
    printf("\n");

    /* 3. Signed/Unsigned Hazard Demonstration */
    demonstrate_signed_unsigned_hazard(-5, 10u);

    printf("==============================================================\n");
    return EXIT_SUCCESS;
}

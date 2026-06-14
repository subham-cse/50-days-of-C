/**
 * ============================================================================
 * DAY 08: Bitwise Operators, Shift Mechanics, and Bitfield Manipulation
 * ============================================================================
 * Description:
 *   Demonstrates low-level bitwise operations (&, |, ^, ~, <<, >>), binary bit
 *   inspection, flag register manipulation (set, clear, toggle), Brian Kernighan's
 *   population count algorithm, and power-of-two validation.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 08_Bitwise_Shift_Operators_code.c -o day08_bitwise
 *
 * Execution:
 *   ./day08_bitwise
 *
 * Complexity Metrics:
 *   - Time Complexity: O(K) where K is number of set bits (Kernighan's PopCount)
 *   - Space Complexity: O(1) auxiliary stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 1. HARDWARE REGISTER BIT MASK DEFINITIONS
 * ============================================================================
 */

#define REG_POWER_ON_BIT   (1U << 0)  /* Bit 0: Device Power Enable */
#define REG_TX_READY_BIT   (1U << 1)  /* Bit 1: Transmitter Ready   */
#define REG_RX_READY_BIT   (1U << 2)  /* Bit 2: Receiver Data Avail */
#define REG_ERR_INTERRUPT  (1U << 3)  /* Bit 3: Hardware Error Flag */
#define REG_MODE_MASK      (0x3U << 4)/* Bits 4-5: Operating Mode   */

/* ============================================================================
 * 2. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * print_binary_32:
 *   Formats and displays a 32-bit unsigned integer in human-readable binary
 *   grouped by 4-bit nibbles.
 *
 * Parameters:
 *   value - 32-bit unsigned integer to display.
 *   label - Descriptive label prefix.
 *
 * Returns:
 *   void
 */
void print_binary_32(uint32_t value, const char *label)
{
    printf("  %-22s : 0b", label);
    for (int i = 31; i >= 0; --i) {
        putchar((value & (1U << i)) ? '1' : '0');
        if (i % 4 == 0 && i != 0) {
            putchar(' ');
        }
    }
    printf(" (Hex: 0x%08X | Dec: %u)\n", value, value);
}

/**
 * count_set_bits_kernighan:
 *   Computes the population count (Hamming weight) of a 32-bit integer in
 *   O(K) iterations, where K is the number of active 1-bits.
 *
 * Parameters:
 *   n - Integer value to analyze.
 *
 * Returns:
 *   uint32_t - Total count of set bits.
 */
uint32_t count_set_bits_kernighan(uint32_t n)
{
    uint32_t count = 0;
    while (n != 0) {
        n = n & (n - 1); /* Clears the lowest set bit in a single cycle */
        count++;
    }
    return count;
}

/**
 * is_power_of_two:
 *   Determines if a positive integer is an exact power of two using a
 *   constant-time O(1) bitwise trick.
 *
 * Parameters:
 *   val - Unsigned 32-bit integer.
 *
 * Returns:
 *   bool - True if val is 2^k (k >= 0), False otherwise.
 */
bool is_power_of_two(uint32_t val)
{
    return (val != 0) && ((val & (val - 1)) == 0);
}

/* ============================================================================
 * 3. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 08: Bitwise Operators, Shift Mechanics & Flag Control\n");
    printf("==============================================================\n\n");

    /* 1. Bitwise Masking Operations */
    printf("--- 1. Hardware Status Register Simulation ---\n");
    uint32_t status_reg = 0;
    print_binary_32(status_reg, "Initial Register");

    /* Set Bit 0 (Power) and Bit 1 (TX Ready) */
    status_reg |= (REG_POWER_ON_BIT | REG_TX_READY_BIT);
    print_binary_32(status_reg, "Power & TX Enabled");

    /* Toggle Bit 3 (Error Interrupt) */
    status_reg ^= REG_ERR_INTERRUPT;
    print_binary_32(status_reg, "Error Bit Toggled ON");

    /* Clear Bit 1 (TX Ready) */
    status_reg &= ~REG_TX_READY_BIT;
    print_binary_32(status_reg, "TX Ready Cleared");

    /* Test Bit 0 */
    bool is_power = (status_reg & REG_POWER_ON_BIT) != 0;
    printf("  >> Query Bit 0 (Power State): %s\n\n", is_power ? "ONLINE" : "OFFLINE");

    /* 2. Population Count (Brian Kernighan's Algorithm) */
    printf("--- 2. Population Count (Hamming Weight) ---\n");
    uint32_t test_values[] = {0, 1, 7, 0xAA, 0xFFFFFFFF, 0x12345678};
    for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); ++i) {
        uint32_t val = test_values[i];
        printf("  PopCount(0x%08X) = %2u bits set\n", val, count_set_bits_kernighan(val));
    }
    printf("\n");

    /* 3. Power of 2 Testing */
    printf("--- 3. Constant-Time Power-of-Two Detection ---\n");
    uint32_t pow2_candidates[] = {0, 1, 2, 3, 4, 16, 18, 1024, 1025};
    for (size_t i = 0; i < sizeof(pow2_candidates) / sizeof(pow2_candidates[0]); ++i) {
        uint32_t c = pow2_candidates[i];
        printf("  Value %5u -> Power of Two: %s\n", c, is_power_of_two(c) ? "YES (True)" : "NO  (False)");
    }
    printf("==============================================================\n");

    return EXIT_SUCCESS;
}

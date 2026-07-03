/**
 * ============================================================================
 * Day 27: Call by Value vs Call by Reference (via Pointers)
 * File: 27_Call_By_Value_vs_Reference_code.c
 * 
 * Overview:
 * Comprehensive comparison between value passing (isolation) and address
 * passing (mutation & multiple return values), including swap implementations,
 * defensive null checks, and large structure pass-by-const-pointer optimization.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 27_Call_By_Value_vs_Reference_code.c -o 27_Call_By_Value_vs_Reference_code
 * 
 * Complexity:
 * - Call by value scalar: O(1) time, O(1) space copy
 * - Call by pointer:      O(1) time, O(1) address pass
 * ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    double sensor_readings[1024]; // 8 KB payload
    uint32_t device_id;
    bool is_calibrated;
} TelemetryPacket;

/**
 * Helper 1: Failed Swap (Call by Value)
 * Modifies local copies on callee stack frame; leaves caller untouched.
 */
void swap_by_value(int a, int b) {
    int temp = a;
    a = b;
    b = temp;
    printf("    [Inside swap_by_value] a = %d, b = %d (local stack only)\n", a, b);
}

/**
 * Helper 2: Successful Swap (Call by Reference via Pointers)
 * Dereferences caller memory addresses directly.
 */
void swap_by_reference(int *a, int *b) {
    if (!a || !b) return; // Defensive null guard
    int temp = *a;
    *a = *b;
    *b = temp;
    printf("    [Inside swap_by_reference] *a = %d, *b = %d (caller mutated)\n", *a, *b);
}

/**
 * Helper 3: Multiple Return Values via Output Parameters
 * Computes division quotient and remainder simultaneously.
 */
bool divide_with_remainder(int dividend, int divisor, int *quotient, int *remainder) {
    if (divisor == 0 || !quotient || !remainder) {
        return false; // Error condition
    }
    *quotient = dividend / divisor;
    *remainder = dividend % divisor;
    return true;
}

/**
 * Helper 4: Efficient Struct Processing via Const Pointer
 * Avoids copying 8 KB onto the call stack.
 */
double compute_telemetry_average(const TelemetryPacket *packet) {
    if (!packet) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < 1024; ++i) {
        sum += packet->sensor_readings[i];
    }
    return sum / 1024.0;
}

int main(void) {
    printf("============================================================\n");
    printf("   DAY 27: CALL BY VALUE VS CALL BY REFERENCE IN C          \n");
    printf("============================================================\n\n");

    // 1. Swap Demonstration
    int x = 100, y = 200;
    printf("[+] Initial Values: x = %d, y = %d\n\n", x, y);

    printf("[+] Attempting swap_by_value(x, y)...\n");
    swap_by_value(x, y);
    printf("    [After swap_by_value in main] x = %d, y = %d (Unchanged!)\n\n", x, y);

    printf("[+] Attempting swap_by_reference(&x, &y)...\n");
    swap_by_reference(&x, &y);
    printf("    [After swap_by_reference in main] x = %d, y = %d (Successfully Swapped!)\n\n", x, y);

    // 2. Multiple Return Value Pattern (Quotient & Remainder)
    int num = 47, den = 5;
    int q = 0, r = 0;
    printf("[+] Demonstrating Multiple Returns via Output Pointers:\n");
    if (divide_with_remainder(num, den, &q, &r)) {
        printf("    %d / %d => Quotient = %d, Remainder = %d\n\n", num, den, q, r);
    }

    // 3. Error Case Handling (Division by Zero and Null Pointer)
    printf("[+] Testing Division by Zero Guard:\n");
    bool status_zero = divide_with_remainder(num, 0, &q, &r);
    printf("    Division by 0 status: %s\n", status_zero ? "SUCCESS" : "REJECTED (Safe)");

    bool status_null = divide_with_remainder(num, den, NULL, &r);
    printf("    Null pointer status:  %s\n\n", status_null ? "SUCCESS" : "REJECTED (Safe)");

    // 4. Large Struct Passing Performance Comparison
    TelemetryPacket data;
    data.device_id = 4201;
    data.is_calibrated = true;
    for (int i = 0; i < 1024; ++i) {
        data.sensor_readings[i] = (double)(i * 2);
    }

    printf("[+] Efficient Struct Passing via Const Pointer:\n");
    printf("    TelemetryPacket Size: %zu bytes\n", sizeof(data));
    double avg = compute_telemetry_average(&data);
    printf("    Computed Average Sensor Reading: %.2f\n", avg);

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}

/**
 * ============================================================================
 * Day 35: Storage Classes — static and extern
 * File: 35_Static_Extern_Classes_code.c
 * 
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 35_Static_Extern_Classes_code.c -o 35_Static_Extern_Classes_code
 * 
 * Description:
 *   Demonstrates persistent local static counters, file-scoped static variables
 *   with internal linkage (encapsulation), extern declarations for global
 *   linkage across scopes, and memory segment address verification.
 * 
 * Complexity:
 *   Time Complexity:  O(1) memory accesses
 *   Space Complexity: O(1) in Data/BSS segments
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Global variable with external linkage (resides in .data segment) */
int g_shared_counter = 1000;

/* File-scope static variable with internal linkage (visible ONLY in this translation unit) */
static int s_private_system_id = 42;

/* Uninitialized global (resides in .bss segment, automatically zeroed) */
int g_uninit_zero_var;

/**
 * @brief Demonstrates local static state retention across multiple function calls.
 * 
 * @param transaction_amount Value to add to the persistent account balance.
 * @return Updated running total.
 */
double process_account_transaction(double transaction_amount) {
    /* 
     * Local static variable:
     * - Initialized ONLY ONCE when program loads.
     * - Persists between function invocations.
     * - Stored in .data segment, NOT on the stack frame.
     */
    static double s_account_balance = 500.0; /* Initial deposit */
    static uint32_t s_transaction_id = 0;

    s_transaction_id++;
    s_account_balance += transaction_amount;

    printf("  [Transaction #%u] Delta: %+.2f | Current Balance: $%.2f (Balance Addr: %p)\n",
           s_transaction_id, transaction_amount, s_account_balance, (void*)&s_account_balance);

    return s_account_balance;
}

/**
 * @brief Demonstrates private file-scoped static function (Internal Linkage).
 */
static void internal_system_diagnostics(void) {
    printf("--- [Internal Module Diagnostics (static function)] ---\n");
    printf("Internal System ID: %d (Addr: %p)\n", s_private_system_id, (void*)&s_private_system_id);
    printf("Global Shared Var:  %d (Addr: %p)\n", g_shared_counter, (void*)&g_shared_counter);
    printf("BSS Zero Variable:  %d (Addr: %p)\n\n", g_uninit_zero_var, (void*)&g_uninit_zero_var);
}

/**
 * @brief Demonstrates 'extern' block-level declaration resolving to global symbol.
 */
void demonstrate_extern_resolution(void) {
    /* Explicit block-level extern declaration referencing global identifier */
    extern int g_shared_counter;
    
    printf("--- [Block-Level extern Reference] ---\n");
    printf("Accessed g_shared_counter via extern: %d\n", g_shared_counter);
    g_shared_counter += 50;
    printf("Modified g_shared_counter to:        %d\n\n", g_shared_counter);
}

/**
 * @brief Inspects physical memory layout segments of different storage classes.
 */
void inspect_memory_segments(void) {
    int local_stack_var = 10;
    static int local_static_var = 20;

    printf("--- [Memory Segment Address Inspection] ---\n");
    printf(".TEXT (Code) Address of main():               %p\n", (void*)(uintptr_t)main);
    printf(".DATA Address of g_shared_counter (global):   %p\n", (void*)&g_shared_counter);
    printf(".DATA Address of s_private_system_id (static):%p\n", (void*)&s_private_system_id);
    printf(".DATA Address of local_static_var:            %p\n", (void*)&local_static_var);
    printf(".BSS  Address of g_uninit_zero_var:           %p\n", (void*)&g_uninit_zero_var);
    printf("STACK Address of local_stack_var:             %p\n", (void*)&local_stack_var);
    printf("Note: Stack address is far higher than Data/BSS/Text in virtual memory.\n\n");
}

int main(void) {
    printf("===============================================================\n");
    printf("     DAY 35: STORAGE CLASSES — STATIC AND EXTERN\n");
    printf("===============================================================\n\n");

    /* 1. Local static persistence across calls */
    printf("[1] Local Static State Persistence Across Calls:\n");
    process_account_transaction(150.00);
    process_account_transaction(-50.00);
    process_account_transaction(320.75);
    process_account_transaction(-120.25);
    printf("\n");

    /* 2. File-scope static and internal linkage */
    printf("[2] Internal Linkage & File-Scope Static Variables:\n");
    internal_system_diagnostics();

    /* 3. Extern declaration */
    printf("[3] Extern Linkage Scope Demonstration:\n");
    demonstrate_extern_resolution();

    /* 4. Memory segment topology */
    printf("[4] Memory Segment Topography:\n");
    inspect_memory_segments();

    return 0;
}

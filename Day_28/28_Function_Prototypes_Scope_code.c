/**
 * ============================================================================
 * Day 28: Function Prototypes, Scope, and Storage Classes
 * File: 28_Function_Prototypes_Scope_code.c
 * 
 * Overview:
 * In-depth demonstration of variable scope resolution (block, file, prototype),
 * storage classes (auto, static, extern, register), variable shadowing,
 * internal linkage (static functions/globals), and persistent state counters.
 * 
 * Compilation:
 * gcc -Wall -Wextra -std=c99 28_Function_Prototypes_Scope_code.c -o 28_Function_Prototypes_Scope_code
 * 
 * Complexity:
 * - Function Calls: O(1) time, O(1) stack allocation
 * - Static Counter Accumulation: O(1) time, O(1) permanent .bss memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// ----------------------------------------------------------------------------
// File Scope (Global Variables & Prototypes)
// ----------------------------------------------------------------------------
int g_global_counter = 100;              // External linkage (public global)
static int s_file_private_id = 9999;     // Internal linkage (file-scope private)

// Forward declarations / prototypes
void demonstrate_scope_shadowing(void);
uint64_t generate_unique_transaction_id(void);
static void log_internal_event(const char *event_name);
int compute_fast_sum(int limit);

// ----------------------------------------------------------------------------
// Main Driver Function
// ----------------------------------------------------------------------------
int main(void) {
    printf("============================================================\n");
    printf("   DAY 28: FUNCTION PROTOTYPES, SCOPES & STORAGE CLASSES    \n");
    printf("============================================================\n\n");

    // 1. Inspecting File Scope Globals
    printf("[+] File Scope Globals:\n");
    printf("    g_global_counter:    %d (Address: %p)\n", g_global_counter, (void*)&g_global_counter);
    printf("    s_file_private_id:   %d (Address: %p)\n\n", s_file_private_id, (void*)&s_file_private_id);

    // 2. Block Scope & Variable Shadowing Demonstration
    demonstrate_scope_shadowing();
    printf("\n");

    // 3. Static Local Storage Duration (Persistent State Across Invocations)
    printf("[+] Demonstrating Static Local Variable (Persistent Lifetime):\n");
    for (int i = 1; i <= 4; ++i) {
        uint64_t tx_id = generate_unique_transaction_id();
        printf("    Call %d -> Generated Transaction ID: %llu\n", i, (unsigned long long)tx_id);
    }
    printf("\n");

    // 4. Register Storage Class Demonstration
    printf("[+] Demonstrating Register Optimization Hint:\n");
    int sum = compute_fast_sum(1000);
    printf("    Sum of numbers 1..1000: %d\n\n", sum);

    // 5. Internal Linkage Helper Function Call
    log_internal_event("SYSTEM_INITIALIZATION_SUCCESS");

    printf("\n============================================================\n");
    printf("   Execution completed successfully.                        \n");
    printf("============================================================\n");

    return 0;
}

// ----------------------------------------------------------------------------
// Function Definitions
// ----------------------------------------------------------------------------

/**
 * Helper 1: Scope Masking & Block-Level Shadowing
 */
void demonstrate_scope_shadowing(void) {
    int level = 1; // Outer function block scope
    printf("[+] Demonstrating Scope Shadowing:\n");
    printf("    Level at outer block:       %d (Addr: %p)\n", level, (void*)&level);

    {
        int level = 2; // Shadows outer level
        printf("    Level at inner block 1:     %d (Addr: %p - SHADOWED)\n", level, (void*)&level);

        {
            int level = 3; // Shadows inner block 1 level
            printf("    Level at deepest nested:    %d (Addr: %p - SHADOWED)\n", level, (void*)&level);
        }

        printf("    Level exiting inner block:  %d (Addr: %p)\n", level, (void*)&level);
    }

    printf("    Level back in outer block:  %d (Addr: %p - RESTORED)\n", level, (void*)&level);
}

/**
 * Helper 2: Static Local Variable Generator
 * The variable 's_sequence' retains its memory value across multiple invocations.
 */
uint64_t generate_unique_transaction_id(void) {
    // Initialized only ONCE at program load time, not on each call
    static uint64_t s_sequence = 100000;
    return ++s_sequence;
}

/**
 * Helper 3: Fast Accumulator with register storage class
 */
int compute_fast_sum(int limit) {
    register int accumulator = 0;
    for (register int i = 1; i <= limit; ++i) {
        accumulator += i;
    }
    return accumulator;
}

/**
 * Helper 4: File-private (internal linkage) logging function
 */
static void log_internal_event(const char *event_name) {
    printf("[LOG - File Scope ID: %d] Event: %s\n", s_file_private_id, event_name);
}

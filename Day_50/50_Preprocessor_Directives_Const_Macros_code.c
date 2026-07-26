/**
 * ============================================================================
 * Day 50: Preprocessor Directives, Macros, and #define vs. const
 * File: 50_Preprocessor_Directives_Const_Macros_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 50_Preprocessor_Directives_Const_Macros_code.c -o 50_preprocessor
 *
 * Concepts Demonstrated:
 *   1. Function-like macros with defensive parenthesization.
 *   2. Stringification operator (#) and Token Concatenation operator (##).
 *   3. Robust multi-statement macros using the do { ... } while(0) idiom.
 *   4. Side-by-side comparison between #define and const variables.
 *   5. Conditional compilation directives (#ifdef, #if, #else).
 *
 * Complexity:
 *   - Preprocessor Operations: Compile-time O(0) runtime overhead.
 *   - Macro Execution: O(1) inlined operations.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* --- 1. Constant Definitions: #define vs const --- */
#define BUFFER_SIZE_MACRO 128
static const size_t BufferSizeConst = 128;

/* --- 2. Safe Function-Like Macros --- */
#define SAFE_SQUARE(x)       ((x) * (x))
#define SAFE_MAX(a, b)       (((a) > (b)) ? (a) : (b))
#define ARRAY_COUNT(arr)     (sizeof(arr) / sizeof((arr)[0]))

/* --- 3. Stringification (#) & Token Concatenation (##) --- */
#define DUMP_VAR(var) \
    printf("    [DUMP] %s = %d (Address: %p)\n", #var, (var), (void *)&(var))

#define DEFINE_HANDLER(event_name) \
    void handle_##event_name(void) { \
        printf("    [EVENT HANDLER] Dispatched event: " #event_name "\n"); \
    }

/* Generate event handlers at compile-time via token pasting */
DEFINE_HANDLER(network_connect)
DEFINE_HANDLER(disk_write_complete)

/* --- 4. Multi-Statement Macro Idiom --- */
static int g_log_counter = 0;
#define SAFE_LOG(level, msg) do { \
    printf("    [%s] (Log #%d): %s\n", (level), ++g_log_counter, (msg)); \
} while (0)

/**
 * demonstrate_macro_precedence - Compares safe vs unsafe macro expansion.
 */
void demonstrate_macro_precedence(void) {
    printf("[1] Macro Precedence & Operator Order:\n");
    int a = 3, b = 2;

    /* SAFE_SQUARE(3 + 2) expands to ((3 + 2) * (3 + 2)) = 25 */
    int result_safe = SAFE_SQUARE(a + b);
    printf("    SAFE_SQUARE(%d + %d) = %d (Expected: 25)\n", a, b, result_safe);

    int max_val = SAFE_MAX(a * 4, b * 10);
    printf("    SAFE_MAX(%d, %d) = %d\n\n", a * 4, b * 10, max_val);
}

/**
 * demonstrate_const_vs_define - Highlights type safety and memory of const.
 */
void demonstrate_const_vs_define(void) {
    printf("[2] #define vs static const Comparison:\n");
    printf("    #define BUFFER_SIZE_MACRO : %d (Text token; unaddressable)\n", BUFFER_SIZE_MACRO);
    printf("    static const size_t Const : %zu (Addressable at %p)\n\n",
           BufferSizeConst, (void *)&BufferSizeConst);
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 50: PREPROCESSOR DIRECTIVES, CONST & MACROS  \n");
    printf("====================================================\n\n");

    /* Section 1: Macro Precedence */
    demonstrate_macro_precedence();

    /* Section 2: Const vs Define */
    demonstrate_const_vs_define();

    /* Section 3: Stringification and Variable Dumping */
    printf("[3] Stringification Operator (#):\n");
    int sensor_pressure = 1013;
    int battery_percentage = 98;
    DUMP_VAR(sensor_pressure);
    DUMP_VAR(battery_percentage);
    printf("\n");

    /* Section 4: Token Concatenation Operator (##) */
    printf("[4] Token Concatenation Operator (##):\n");
    handle_network_connect();
    handle_disk_write_complete();
    printf("\n");

    /* Section 5: Multi-statement macro safety in conditional blocks */
    printf("[5] Multi-Statement do { ... } while(0) Macro in if-else:\n");
    bool system_healthy = true;

    if (system_healthy)
        SAFE_LOG("INFO", "All health checks passed.");
    else
        SAFE_LOG("ERROR", "Subsystem failure detected.");

    SAFE_LOG("DEBUG", "Pre-flight calibration verified.");
    printf("\n");

    /* Section 6: Standard Predefined Preprocessor Macros */
    printf("[6] Standard ANSI C Predefined Environment Macros:\n");
    printf("    Compiled File : %s\n", __FILE__);
    printf("    Compiled Date : %s\n", __DATE__);
    printf("    Compiled Time : %s\n", __TIME__);
    printf("    C Standard Ver: %ld\n", (long)__STDC_VERSION__);

    /* Section 7: Array Count Macro */
    int values[] = { 10, 20, 30, 40, 50, 60, 70 };
    printf("    ARRAY_COUNT(values) = %zu elements\n", ARRAY_COUNT(values));

    printf("\n====================================================\n");
    printf("   CONGRATULATIONS: 50 DAYS OF C COMPLETED!         \n");
    printf("====================================================\n");

    return 0;
}

/**
 * ============================================================================
 * DAY 01: Introduction, History, and Structural Anatomy of a C Program
 * ============================================================================
 * Description:
 *   Demonstrates the structural components of a standard-compliant C translation
 *   unit: file-scope declarations, static global storage, command-line parsing,
 *   activation frame behavior on the stack, and exit status signaling.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 01_Intro_History_Structure_code.c -o day01_intro
 *
 * Execution:
 *   ./day01_intro [arg1 arg2 ...]
 *
 * Complexity Metrics:
 *   - Time Complexity: O(N) where N is the number of command line arguments
 *   - Space Complexity: O(1) auxiliary stack space per invocation frame
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================================
 * 1. FILE-SCOPE CONSTANTS & DATA SEGMENT DECLARATIONS
 * ============================================================================
 */

#define PROGRAM_VERSION "1.0.0"
#define MAX_BUFFER_SIZE 256

/* Statically allocated initialized global -> resides in .data segment */
static const char g_author_info[] = "Dennis Ritchie / ISO C99 Conforming Standard";

/* Statically allocated uninitialized global -> resides in .bss segment */
static uint32_t g_execution_counter;

/* ============================================================================
 * 2. AUXILIARY / HELPER FUNCTION DECLARATIONS & DEFINITIONS
 * ============================================================================
 */

/**
 * inspect_memory_segments:
 *   Illustrates relative virtual address offsets across the major process
 *   memory segments: .rodata/.text, .data, .bss, and the stack frame.
 *
 * Parameters:
 *   sample_param - An integer parameter passed on the activation stack frame.
 *
 * Returns:
 *   void
 */
void inspect_memory_segments(int sample_param)
{
    /* Local automatic variable -> allocated on the current activation stack frame */
    int local_stack_var = 1337;
    const char *literal_str = "String literal in .rodata";

    printf("\n=== Process Virtual Address Space Inspection ===\n");
    printf(" [.text / Code Entry]      inspect_memory_segments : %p\n", (void *)(uintptr_t)inspect_memory_segments);
    printf(" [.rodata / String Const]  literal_str             : %p\n", (void *)literal_str);
    printf(" [.data / Initialized]     g_author_info           : %p\n", (void *)g_author_info);
    printf(" [.bss / Zero-Init]        g_execution_counter     : %p\n", (void *)&g_execution_counter);
    printf(" [Stack / Function Param]  sample_param            : %p\n", (void *)&sample_param);
    printf(" [Stack / Local Auto Var]  local_stack_var         : %p\n", (void *)&local_stack_var);
    printf("================================================\n\n");
}

/**
 * parse_cli_arguments:
 *   Iterates through and safely validates the command line argument vector
 *   provided by the host runtime environment.
 *
 * Parameters:
 *   argc - Total argument count including executable invocation path.
 *   argv - Null-terminated array of string pointers.
 *
 * Returns:
 *   size_t - Total aggregate character count of all user arguments.
 */
size_t parse_cli_arguments(int argc, char *argv[])
{
    size_t total_chars = 0;

    printf("=== Command Line Argument Vector Analysis ===\n");
    printf("Total Argument Count (argc): %d\n", argc);

    for (int i = 0; i < argc; ++i) {
        /* Verify standard guarantee: argv[i] must not be NULL when i < argc */
        if (argv[i] != NULL) {
            size_t len = strlen(argv[i]);
            total_chars += len;
            printf("  argv[%d] (addr: %p) -> \"%s\" [len: %zu bytes]\n", 
                   i, (void *)argv[i], argv[i], len);
        }
    }

    /* Standard guarantee check: argv[argc] is guaranteed to be NULL */
    if (argv[argc] == NULL) {
        printf("  argv[%d] (terminator) -> NULL (ISO C Standard Compliant)\n", argc);
    }
    printf("=============================================\n\n");

    return total_chars;
}

/* ============================================================================
 * 3. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(int argc, char *argv[])
{
    g_execution_counter = 1;

    printf("************************************************************\n");
    printf("  Day 01: C Program Anatomy & Architecture Demo v%s\n", PROGRAM_VERSION);
    printf("  Reference Spec: %s\n", g_author_info);
    printf("************************************************************\n");

    /* Helper Function 1: Inspect memory layout */
    inspect_memory_segments(42);

    /* Helper Function 2: Parse and validate CLI arguments */
    size_t char_count = parse_cli_arguments(argc, argv);
    printf("[Diagnostic] Aggregate length of argument strings: %zu characters.\n", char_count);

    /* Demonstrate execution counter increment */
    g_execution_counter++;
    printf("[Status] Execution completed successfully with run count = %u.\n", g_execution_counter);

    /* Return standard success code to operating system */
    return EXIT_SUCCESS;
}

/**
 * ============================================================================
 * DAY 02: Execution Flow, Translators, and Compilation Pipeline Mechanics
 * ============================================================================
 * Description:
 *   Demonstrates translation-unit-level concepts: preprocessor transformations,
 *   internal vs. external linkage, symbol visibility, and execution flow tracking
 *   across distinct functional boundaries.
 *
 * Compilation:
 *   gcc -Wall -Wextra -Wpedantic -std=c99 02_Execution_Flow_Translators_code.c -o day02_execution_flow
 *
 * Execution:
 *   ./day02_execution_flow
 *
 * Complexity Metrics:
 *   - Time Complexity: O(N) where N is the number of execution steps
 *   - Space Complexity: O(1) auxiliary frame stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * 1. PREPROCESSOR METADATA & MACRO GUARDS
 * ============================================================================
 */

#define COMPILER_PIPELINE_STAGES 4
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

/* Compile-time conditional configuration */
#ifdef __STDC_VERSION__
    #define C_STANDARD_DESC "Standard: ISO/IEC 9899:" STRINGIFY(__STDC_VERSION__)
#else
    #define C_STANDARD_DESC "Standard: Pre-C89 / Legacy"
#endif

/* ============================================================================
 * 2. SYMBOL VISIBILITY & LINKAGE DEMONSTRATION
 * ============================================================================
 */

/* Internal Linkage: Scoped strictly to this translation unit (.symtab: LOCAL) */
static uint32_t s_local_invocation_counter = 0;

/* External Linkage: Exported symbol visible to linker (.symtab: GLOBAL) */
const char g_pipeline_stage_names[COMPILER_PIPELINE_STAGES][24] = {
    "1. Preprocessing",
    "2. Compilation Proper",
    "3. Assembly",
    "4. Linking"
};

/* ============================================================================
 * 3. AUXILIARY / HELPER FUNCTIONS
 * ============================================================================
 */

/**
 * log_pipeline_stage:
 *   Simulates and logs the transformation artifacts generated during each
 *   compilation stage.
 *
 * Parameters:
 *   stage_idx   - Zero-based index corresponding to the compilation stage.
 *   tool_name   - The Unix/GNU toolchain component executing the stage.
 *   input_file  - Source file extension or artifact consumed.
 *   output_file - Resulting intermediate representation or binary artifact.
 *
 * Returns:
 *   bool - True if index is within valid bounds, False otherwise.
 */
bool log_pipeline_stage(int stage_idx, const char *tool_name, 
                        const char *input_file, const char *output_file)
{
    if (stage_idx < 0 || stage_idx >= COMPILER_PIPELINE_STAGES) {
        return false;
    }

    s_local_invocation_counter++;

    printf("  [%s]\n", g_pipeline_stage_names[stage_idx]);
    printf("    |-- Executable Component : %s\n", tool_name);
    printf("    |-- Input Representation : %s\n", input_file);
    printf("    |-- Emitted Artifact     : %s\n", output_file);
    printf("    `-- Cumulative Step Count: %u\n\n", s_local_invocation_counter);

    return true;
}

/**
 * calculate_checksum_sim:
 *   Demonstrates deterministic runtime code execution and function activation
 *   frame creation, computing a lightweight 32-bit hash of an input buffer.
 *
 * Parameters:
 *   data - Read-only pointer to byte buffer.
 *   size - Buffer length in bytes.
 *
 * Returns:
 *   uint32_t - Computed FNV-1a hash checksum.
 */
uint32_t calculate_checksum_sim(const uint8_t *data, size_t size)
{
    /* 32-bit FNV-1a Initial Offset Basis */
    uint32_t hash = 2166136261u;

    if (data == NULL) {
        return 0;
    }

    for (size_t i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= 16777619u; /* 32-bit FNV prime */
    }

    return hash;
}

/* ============================================================================
 * 4. PROGRAM ENTRY POINT
 * ============================================================================
 */

int main(void)
{
    printf("==============================================================\n");
    printf(" Day 02: Execution Flow & Compilation Translators\n");
    printf(" Environment: %s\n", C_STANDARD_DESC);
    printf(" Compiler File Context: %s (Compiled on: %s at %s)\n", __FILE__, __DATE__, __TIME__);
    printf("==============================================================\n\n");

    printf("[Phase Walkthrough: Source Code to Executable Binary]\n");

    /* Step 1: Preprocessor */
    log_pipeline_stage(0, "cpp (C Preprocessor)", "*.c, *.h", "*.i (Preprocessed translation unit)");

    /* Step 2: Compiler */
    log_pipeline_stage(1, "cc1 (Compiler proper)", "*.i", "*.s (Target architecture assembly)");

    /* Step 3: Assembler */
    log_pipeline_stage(2, "as (Assembler)", "*.s", "*.o (Relocatable machine object)");

    /* Step 4: Linker */
    log_pipeline_stage(3, "ld (Linker / Loader)", "*.o, crt0.o, libc.a/.so", "Executable Binary (.elf/.exe)");

    /* Demonstration of runtime execution checksum */
    const char sample_code_bytes[] = "int main() { return 0; }";
    uint32_t sample_hash = calculate_checksum_sim((const uint8_t *)sample_code_bytes, sizeof(sample_code_bytes) - 1);

    printf("=== Runtime Execution Diagnostic ===\n");
    printf("Sample Input Buffer : \"%s\"\n", sample_code_bytes);
    printf("Calculated Hash     : 0x%08X\n", sample_hash);
    printf("Total Pipeline Calls: %u\n", s_local_invocation_counter);
    printf("====================================\n");

    return EXIT_SUCCESS;
}

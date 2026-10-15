/**
 * ============================================================================
 * 50 Days of C - Verification Test Harness: Multi-Threaded Verifier
 * File: harness/verifier.c
 * Standard: POSIX.1-2008 / ISO C99
 * ============================================================================
 * Description:
 *   High-performance concurrent verification harness across all 50 days of C.
 *   Implements:
 *     1. Multi-threaded test runner with POSIX threads (pthread_create, pthread_join).
 *     2. Thread-safe work queue with mutual exclusion (pthread_mutex_t).
 *     3. Process sandboxing and external I/O resilience using fork(), execvp(),
 *        output suppression (freopen), and non-blocking timeout supervision
 *        via waitpid(..., WNOHANG) to terminate runaway or hung processes.
 *     4. High-resolution timing metrics via gettimeofday().
 * ============================================================================
 */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#define NULL_DEVICE "NUL"
#else
#include <sys/wait.h>
#include <signal.h>
#define NULL_DEVICE "/dev/null"
#endif

#define DEFAULT_NUM_THREADS 4
#define COMPILE_TIMEOUT_MS  10000
#define EXECUTE_TIMEOUT_MS  3000
#define TOTAL_DAYS          50

/* ============================================================================
 * 1. DATA STRUCTURES & TASK DEFINITIONS
 * ============================================================================
 */

typedef enum {
    TEST_PENDING = 0,
    TEST_PASSED,
    TEST_FAILED_COMPILE,
    TEST_FAILED_EXECUTION,
    TEST_TIMED_OUT,
    TEST_SKIPPED
} TestStatus;

typedef struct {
    int         day_num;
    const char *dir_name;
    const char *file_name;
    const char *title;
} DayTask;

typedef struct {
    int        day_num;
    const char *title;
    TestStatus status;
    long long  compile_time_us;
    long long  run_time_us;
    int        exit_code;
    char       diagnostic[256];
} TestResult;

/* Static registry of all 50 curriculum lessons */
static const DayTask g_curriculum[TOTAL_DAYS] = {
    { 1, "Day_01", "01_Intro_History_Structure_code.c", "Intro, History & Structure" },
    { 2, "Day_02", "02_Execution_Flow_Translators_code.c", "Execution Flow & Translators" },
    { 3, "Day_03", "03_Keywords_Syntax_code.c", "Keywords & ISO Syntax" },
    { 4, "Day_04", "04_Tokens_Identifiers_Constants_code.c", "Tokens, Identifiers & Constants" },
    { 5, "Day_05", "05_DataTypes_Modifiers_Qualifiers_code.c", "Data Types, Modifiers & Qualifiers" },
    { 6, "Day_06", "06_IO_EscapeSequences_code.c", "Standard I/O & Escape Sequences" },
    { 7, "Day_07", "07_Arithmetic_Relational_Logical_code.c", "Arithmetic, Relational & Logical" },
    { 8, "Day_08", "08_Bitwise_Shift_Operators_code.c", "Bitwise & Shift Operators" },
    { 9, "Day_09", "09_Ternary_Sizeof_Comma_code.c", "Ternary, Sizeof & Comma Operator" },
    { 10, "Day_10", "10_Precedence_Expression_Eval_code.c", "Precedence & Expression Evaluation" },
    { 11, "Day_11", "11_If_Else_Nested_code.c", "If / Else & Nested Branching" },
    { 12, "Day_12", "12_Else_If_Ladder_code.c", "Else-If Selection Ladders" },
    { 13, "Day_13", "13_Switch_Case_Control_code.c", "Switch Case & Jump Tables" },
    { 14, "Day_14", "14_While_Loop_code.c", "While Loop & Invariant Conditions" },
    { 15, "Day_15", "15_For_Loop_code.c", "For Loop & Scope Iteration" },
    { 16, "Day_16", "16_Do_While_Loop_code.c", "Do-While Loop & Menu Workflows" },
    { 17, "Day_17", "17_Break_Continue_Goto_code.c", "Break, Continue & Labeled Goto" },
    { 18, "Day_18", "18_Math_Arithmetic_Powers_Sqrt_code.c", "Math Arithmetic, Powers & Sqrt" },
    { 19, "Day_19", "19_Math_Logs_Trig_Floor_Ceil_code.c", "Math Logs, Trig, Floor & Ceil" },
    { 20, "Day_20", "20_1D_Arrays_Basics_code.c", "1D Arrays & Memory Decay" },
    { 21, "Day_21", "21_2D_Arrays_Matrices_code.c", "2D Arrays & Matrix Memory Layout" },
    { 22, "Day_22", "22_Matrix_Operations_code.c", "Matrix Arithmetic & Transformations" },
    { 23, "Day_23", "23_Strings_Basics_IO_code.c", "Strings Basics & Safe Buffers" },
    { 24, "Day_24", "24_Strlen_Strcpy_Strcat_Strcmp_code.c", "String.h Core Utility Functions" },
    { 25, "Day_25", "25_Strrev_Case_Transformations_code.c", "In-Place Reversal & Case Transforms" },
    { 26, "Day_26", "26_User_Defined_Functions_code.c", "User-Defined Functions & Signatures" },
    { 27, "Day_27", "27_Call_By_Value_vs_Reference_code.c", "Call By Value vs Pointer Reference" },
    { 28, "Day_28", "28_Function_Prototypes_Scope_code.c", "Function Prototypes & Storage Scope" },
    { 29, "Day_29", "29_Recursion_Factorial_code.c", "Recursion Mechanics & Factorial" },
    { 30, "Day_30", "30_Recursion_Sum_Natural_Numbers_code.c", "Linear Recursion & Natural Sum" },
    { 31, "Day_31", "31_Recursion_GCD_code.c", "Euclidean GCD & Modular Arithmetic" },
    { 32, "Day_32", "32_Recursion_Digit_Sum_Root_code.c", "Digital Root Extraction & Recurrence" },
    { 33, "Day_33", "33_Recursion_Power_Exponentiation_code.c", "Binary Exponentiation (O(log N))" },
    { 34, "Day_34", "34_Auto_Register_Classes_code.c", "Storage Classes: Auto & Register" },
    { 35, "Day_35", "35_Static_Extern_Classes_code.c", "Storage Classes: Static & Extern" },
    { 36, "Day_36", "36_Structures_Basics_Padding_code.c", "Structures & Memory Alignment Padding" },
    { 37, "Day_37", "37_Unions_vs_Structures_code.c", "Unions & Shared Memory Footprints" },
    { 38, "Day_38", "38_Enums_Typedef_code.c", "Enumerations & Typedef Aliases" },
    { 39, "Day_39", "39_Pointers_Syntax_Dereferencing_code.c", "Pointers Syntax & Dereferencing" },
    { 40, "Day_40", "40_Pointer_Arithmetic_Arrays_code.c", "Pointer Arithmetic & Array Duality" },
    { 41, "Day_41", "41_Pointer_To_Pointer_code.c", "Double Pointers & Multi-Level Indirection" },
    { 42, "Day_42", "42_Void_Null_Wild_Dangling_Pointers_code.c", "Void, NULL, Wild & Dangling Pointers" },
    { 43, "Day_43", "43_Malloc_Free_code.c", "Heap Allocation with Malloc & Free" },
    { 44, "Day_44", "44_Calloc_Zero_Init_code.c", "Zero-Initialized Memory with Calloc" },
    { 45, "Day_45", "45_Realloc_Memory_Leaks_code.c", "Dynamic Buffer Resizing with Realloc" },
    { 46, "Day_46", "46_File_Streams_Open_Close_code.c", "File Streams, Handles & Buffer Flushing" },
    { 47, "Day_47", "47_Formatted_IO_Fprintf_Fscanf_code.c", "Formatted File I/O & Scansets" },
    { 48, "Day_48", "48_Binary_Files_Fread_Fwrite_code.c", "Binary File Streams & Random Access" },
    { 49, "Day_49", "49_Command_Line_Arguments_code.c", "AST Parser & Bytecode VM Engine" },
    { 50, "Day_50", "50_Preprocessor_Directives_Const_Macros_code.c", "POSIX Threads, Mutexes & Sandboxing" }
};

/* Thread-safe queue state */
static int             g_next_task_idx = 0;
static pthread_mutex_t g_queue_mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_report_mutex  = PTHREAD_MUTEX_INITIALIZER;
static TestResult      g_results[TOTAL_DAYS];

/* ============================================================================
 * 2. HIGH RESOLUTION TIMER UTILITY
 * ============================================================================
 */

static long long get_timestamp_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((long long)tv.tv_sec * 1000000LL) + (long long)tv.tv_usec;
}

/* ============================================================================
 * 3. PROCESS SANDBOXING & TIMEOUT SUPERVISION
 * ============================================================================
 */

typedef struct {
    int       exit_code;
    bool      timed_out;
    long long duration_us;
} SandboxedExecResult;

/**
 * Executes a program sandboxed in an isolated child process with timeout supervision
 * and complete standard stream suppression.
 */
static SandboxedExecResult run_sandboxed(char *const argv[], int timeout_ms)
{
    SandboxedExecResult res;
    res.exit_code = -1;
    res.timed_out = false;
    res.duration_us = 0;

    long long t_start = get_timestamp_us();

#ifndef _WIN32
    /* POSIX Process Sandboxing via fork/execvp and non-blocking waitpid */
    pid_t pid = fork();

    if (pid < 0) {
        /* Fork failure */
        res.exit_code = -1;
        return res;
    }

    if (pid == 0) {
        /* Child Process: Suppress and isolate all external I/O streams */
        freopen(NULL_DEVICE, "w", stdout);
        freopen(NULL_DEVICE, "w", stderr);
        freopen(NULL_DEVICE, "r", stdin);

        execvp(argv[0], argv);
        /* If execvp returns, it failed */
        _exit(127);
    }

    /* Parent Process: Non-blocking supervision loop */
    int elapsed_ms = 0;
    const int step_sleep_ms = 10;

    while (elapsed_ms < timeout_ms) {
        int status = 0;
        pid_t w = waitpid(pid, &status, WNOHANG);

        if (w == pid) {
            /* Child process finished cleanly */
            if (WIFEXITED(status)) {
                res.exit_code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                res.exit_code = 128 + WTERMSIG(status);
            }
            res.duration_us = get_timestamp_us() - t_start;
            return res;
        } else if (w < 0) {
            /* Waitpid error */
            break;
        }

        usleep(step_sleep_ms * 1000);
        elapsed_ms += step_sleep_ms;
    }

    /* Process timed out: terminate child forcefully */
    res.timed_out = true;
    kill(pid, SIGKILL);
    int status;
    waitpid(pid, &status, 0);
    res.duration_us = get_timestamp_us() - t_start;
    return res;

#else
    /* Windows fallback execution using system() wrapper with I/O redirect */
    char cmd[1024] = {0};
    snprintf(cmd, sizeof(cmd), "\"%s\" > NUL 2>&1", argv[0]);
    int status = system(cmd);
    res.exit_code = status;
    res.duration_us = get_timestamp_us() - t_start;
    return res;
#endif
}

/* ============================================================================
 * 4. INDIVIDUAL DAY VERIFICATION PIPELINE
 * ============================================================================
 */

static void verify_day_lesson(const DayTask *task, TestResult *result)
{
    result->day_num = task->day_num;
    result->title = task->title;
    result->status = TEST_PENDING;
    result->compile_time_us = 0;
    result->run_time_us = 0;
    result->exit_code = 0;
    result->diagnostic[0] = '\0';

    char src_path[256];
    snprintf(src_path, sizeof(src_path), "%s/%s", task->dir_name, task->file_name);

    /* Verify source file accessibility */
    FILE *fp = fopen(src_path, "r");
    if (!fp) {
        result->status = TEST_SKIPPED;
        snprintf(result->diagnostic, sizeof(result->diagnostic), "File '%s' not found.", src_path);
        return;
    }
    fclose(fp);

    /* Prepare temporary binary target path */
    char bin_path[256];
#ifdef _WIN32
    snprintf(bin_path, sizeof(bin_path), "./.tmp_day_%02d.exe", task->day_num);
#else
    snprintf(bin_path, sizeof(bin_path), "./.tmp_day_%02d_%ld", task->day_num, (long)pthread_self());
#endif

    /* Phase 1: Compilation Phase with ANSI C99 / Wall flags */
    char *compile_argv[] = {
        "gcc",
        "-Wall",
        "-Wextra",
        "-std=c99",
        "-pedantic",
        src_path,
        "-o",
        bin_path,
        "-lm",
        NULL
    };

    SandboxedExecResult compile_res = run_sandboxed(compile_argv, COMPILE_TIMEOUT_MS);
    result->compile_time_us = compile_res.duration_us;

    if (compile_res.timed_out) {
        result->status = TEST_TIMED_OUT;
        snprintf(result->diagnostic, sizeof(result->diagnostic), "Compiler timed out after %d ms.", COMPILE_TIMEOUT_MS);
        return;
    }

    if (compile_res.exit_code != 0) {
        result->status = TEST_FAILED_COMPILE;
        result->exit_code = compile_res.exit_code;
        snprintf(result->diagnostic, sizeof(result->diagnostic), "Compilation failed with exit code %d.", compile_res.exit_code);
        return;
    }

    /* Phase 2: Sandboxed Execution Phase with Timeout Supervision */
    char *exec_argv[] = {
        bin_path,
        NULL
    };

    SandboxedExecResult run_res = run_sandboxed(exec_argv, EXECUTE_TIMEOUT_MS);
    result->run_time_us = run_res.duration_us;
    result->exit_code = run_res.exit_code;

    /* Clean up temporary binary */
    unlink(bin_path);

    if (run_res.timed_out) {
        result->status = TEST_TIMED_OUT;
        snprintf(result->diagnostic, sizeof(result->diagnostic), "Execution timed out after %d ms.", EXECUTE_TIMEOUT_MS);
        return;
    }

    if (run_res.exit_code != 0) {
        result->status = TEST_FAILED_EXECUTION;
        snprintf(result->diagnostic, sizeof(result->diagnostic), "Runtime failure with exit code %d.", run_res.exit_code);
        return;
    }

    result->status = TEST_PASSED;
}

/* ============================================================================
 * 5. WORKER THREAD POOL WORKER
 * ============================================================================
 */

static void *worker_thread_func(void *arg)
{
    long thread_id = (long)arg;

    for (;;) {
        int task_index = -1;

        /* Thread-safe task checkout */
        pthread_mutex_lock(&g_queue_mutex);
        if (g_next_task_idx < TOTAL_DAYS) {
            task_index = g_next_task_idx++;
        }
        pthread_mutex_unlock(&g_queue_mutex);

        if (task_index < 0) {
            break; /* All tasks completed */
        }

        const DayTask *task = &g_curriculum[task_index];
        TestResult result;

        verify_day_lesson(task, &result);

        /* Thread-safe result recording */
        pthread_mutex_lock(&g_report_mutex);
        g_results[task_index] = result;

        const char *status_str = "UNKNOWN";
        switch (result.status) {
            case TEST_PASSED:           status_str = "\033[1;32m[PASS]\033[0m"; break;
            case TEST_FAILED_COMPILE:   status_str = "\033[1;31m[COMPILE FAIL]\033[0m"; break;
            case TEST_FAILED_EXECUTION: status_str = "\033[1;31m[EXEC FAIL]\033[0m"; break;
            case TEST_TIMED_OUT:        status_str = "\033[1;33m[TIMEOUT]\033[0m"; break;
            case TEST_SKIPPED:          status_str = "\033[1;34m[SKIP]\033[0m"; break;
            default: break;
        }

        printf("  [Worker %ld] Day %02d: %-36s %s (%.1f ms)\n",
               thread_id,
               task->day_num,
               task->title,
               status_str,
               (result.compile_time_us + result.run_time_us) / 1000.0);
        fflush(stdout);

        pthread_mutex_unlock(&g_report_mutex);
    }

    return NULL;
}

/* ============================================================================
 * 6. HARNESS ENTRYPOINT & METRIC REPORTING
 * ============================================================================
 */

int main(int argc, char *argv[])
{
    int num_threads = DEFAULT_NUM_THREADS;

    if (argc > 1) {
        int parsed_threads = atoi(argv[1]);
        if (parsed_threads > 0 && parsed_threads <= 32) {
            num_threads = parsed_threads;
        }
    }

    printf("======================================================================\n");
    printf("   50 DAYS OF C: CONCURRENT PROCESS-SANDBOXED TEST HARNESS            \n");
    printf("======================================================================\n");
    printf("Config: %d POSIX Worker Threads | Timeout: %d ms | Targets: %d Days\n\n",
           num_threads, EXECUTE_TIMEOUT_MS, TOTAL_DAYS);

    long long suite_start_us = get_timestamp_us();

    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    if (!threads) {
        fprintf(stderr, "[FATAL] Failed to allocate thread pool.\n");
        return EXIT_FAILURE;
    }

    /* Spawn thread pool */
    for (long i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, worker_thread_func, (void *)(i + 1)) != 0) {
            fprintf(stderr, "[FATAL] Failed to spawn worker thread %ld.\n", i + 1);
            return EXIT_FAILURE;
        }
    }

    /* Await completion */
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);

    long long total_duration_us = get_timestamp_us() - suite_start_us;

    /* Metrics aggregation */
    int count_pass = 0;
    int count_fail_compile = 0;
    int count_fail_exec = 0;
    int count_timeout = 0;
    int count_skip = 0;
    long long total_exec_time_us = 0;

    printf("\n======================================================================\n");
    printf("                       FINAL VERIFICATION AUDIT REPORT                \n");
    printf("======================================================================\n");
    printf("%-7s | %-38s | %-12s | %-10s\n", "Day", "Module Title", "Status", "Duration");
    printf("----------------------------------------------------------------------\n");

    for (int i = 0; i < TOTAL_DAYS; i++) {
        TestResult *r = &g_results[i];
        const char *tag = "UNKNOWN";

        switch (r->status) {
            case TEST_PASSED:
                tag = "PASSED";
                count_pass++;
                break;
            case TEST_FAILED_COMPILE:
                tag = "COMP_FAIL";
                count_fail_compile++;
                break;
            case TEST_FAILED_EXECUTION:
                tag = "EXEC_FAIL";
                count_fail_exec++;
                break;
            case TEST_TIMED_OUT:
                tag = "TIMEOUT";
                count_timeout++;
                break;
            case TEST_SKIPPED:
                tag = "SKIPPED";
                count_skip++;
                break;
            default: break;
        }

        double total_ms = (r->compile_time_us + r->run_time_us) / 1000.0;
        total_exec_time_us += (r->compile_time_us + r->run_time_us);

        printf("Day %02d  | %-38s | %-12s | %7.1f ms\n",
               r->day_num, r->title, tag, total_ms);

        if (r->status != TEST_PASSED && r->status != TEST_PENDING && r->diagnostic[0] != '\0') {
            printf("        -> [Diag]: %s\n", r->diagnostic);
        }
    }

    printf("======================================================================\n");
    printf("METRICS SUMMARY:\n");
    printf("  Total Modules Verified : %d\n", TOTAL_DAYS);
    printf("  Passed                 : %d\n", count_pass);
    printf("  Compilation Failures   : %d\n", count_fail_compile);
    printf("  Execution Failures     : %d\n", count_fail_exec);
    printf("  Timeouts               : %d\n", count_timeout);
    printf("  Skipped                : %d\n", count_skip);
    printf("  Wall-Clock Duration    : %.2f seconds\n", total_duration_us / 1000000.0);
    printf("  Total CPU Work Time    : %.2f seconds\n", total_exec_time_us / 1000000.0);
    if (TOTAL_DAYS > 0) {
        printf("  Average Time per Day   : %.1f ms\n", (total_exec_time_us / 1000.0) / TOTAL_DAYS);
    }
    printf("======================================================================\n");

    return (count_pass == TOTAL_DAYS) ? EXIT_SUCCESS : EXIT_FAILURE;
}

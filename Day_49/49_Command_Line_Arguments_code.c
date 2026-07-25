/**
 * ============================================================================
 * Day 49: Command Line Arguments: argc, argv, and Option Parsing
 * File: 49_Command_Line_Arguments_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 49_Command_Line_Arguments_code.c -o 49_cli_args
 *
 * Usage Examples:
 *   ./49_cli_args --help
 *   ./49_cli_args -v --iterations 5 -o output.log dataset.dat
 *
 * Concepts Demonstrated:
 *   1. Accessing and indexing argc and argv.
 *   2. Production CLI option parser supporting flags, options, and positional args.
 *   3. Robust numerical conversion using strtol with overflow & error checks.
 *   4. Self-testing simulation harness for programmatic verification.
 *
 * Complexity:
 *   - Argument Parsing: O(N) where N is total length of argument characters.
 *   - Space: O(1) auxiliary space.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

/* Configuration structure populated from CLI options */
typedef struct {
    bool verbose;
    int iterations;
    const char *output_file;
    const char *input_file;
} ProgramConfig;

/**
 * print_usage - Prints standard help documentation.
 */
void print_usage(const char *prog_name) {
    printf("Usage: %s [OPTIONS] <input_file>\n\n", prog_name ? prog_name : "program");
    printf("Options:\n");
    printf("  -h, --help               Show this help message and exit\n");
    printf("  -v, --verbose            Enable verbose diagnostic output\n");
    printf("  -i, --iterations <num>   Set iteration count (1 to 1000, default: 1)\n");
    printf("  -o, --output <file>      Specify output filename (default: stdout)\n");
    printf("\nPositional Arguments:\n");
    printf("  input_file               Path to source data file\n");
}

/**
 * safe_parse_int - Converts string to integer with strict error handling.
 * @param str: Input numeric string.
 * @param out_val: Pointer to destination integer.
 * @param min_val: Minimum allowed value.
 * @param max_val: Maximum allowed value.
 * @return: true if valid integer within range, false otherwise.
 */
bool safe_parse_int(const char *str, int *out_val, int min_val, int max_val) {
    if (str == NULL || out_val == NULL) return false;

    char *endptr = NULL;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    /* Check for conversion error or no digits found */
    if (errno != 0 || endptr == str || *endptr != '\0') {
        return false;
    }

    /* Check range bounds */
    if (val < min_val || val > max_val) {
        return false;
    }

    *out_val = (int)val;
    return true;
}

/**
 * parse_arguments - Custom robust command line parser.
 * @return: 0 on success, 1 on error, 2 if help requested.
 */
int parse_arguments(int argc, char *argv[], ProgramConfig *config) {
    if (config == NULL) return 1;

    /* Set default configuration */
    config->verbose = false;
    config->iterations = 1;
    config->output_file = NULL;
    config->input_file = NULL;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            print_usage(argv[0]);
            return 2;
        } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
            config->verbose = true;
        } else if (strcmp(arg, "-i") == 0 || strcmp(arg, "--iterations") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: %s requires an integer argument.\n", arg);
                return 1;
            }
            if (!safe_parse_int(argv[++i], &config->iterations, 1, 1000)) {
                fprintf(stderr, "Error: Invalid iteration count '%s'. Must be 1-1000.\n", argv[i]);
                return 1;
            }
        } else if (strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: %s requires a filename argument.\n", arg);
                return 1;
            }
            config->output_file = argv[++i];
        } else if (arg[0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'. Use --help for usage.\n", arg);
            return 1;
        } else {
            /* Positional argument */
            if (config->input_file == NULL) {
                config->input_file = arg;
            } else {
                fprintf(stderr, "Error: Unexpected extra argument '%s'.\n", arg);
                return 1;
            }
        }
    }

    return 0;
}

/**
 * run_simulation - Tests argument parser with mock argv arrays.
 */
void run_simulation(void) {
    printf("[Demonstrating Command Line Argument Parser Simulation]\n\n");

    /* Simulated scenario 1: Full argument vector */
    char *mock_argv1[] = {
        "simulation_app",
        "--verbose",
        "-i", "5",
        "-o", "results.log",
        "dataset.bin",
        NULL
    };
    int mock_argc1 = 6;

    printf("  Test 1: Parsing Mock CLI -> ./simulation_app --verbose -i 5 -o results.log dataset.bin\n");
    ProgramConfig cfg1;
    if (parse_arguments(mock_argc1, mock_argv1, &cfg1) == 0) {
        printf("    -> Verbose Mode : %s\n", cfg1.verbose ? "ENABLED" : "DISABLED");
        printf("    -> Iterations   : %d\n", cfg1.iterations);
        printf("    -> Output Target: %s\n", cfg1.output_file ? cfg1.output_file : "STDOUT");
        printf("    -> Input File   : %s\n\n", cfg1.input_file ? cfg1.input_file : "NONE");
    }

    /* Simulated scenario 2: Invalid number format */
    char *mock_argv2[] = { "simulation_app", "-i", "invalid_num", NULL };
    int mock_argc2 = 3;
    printf("  Test 2: Parsing Invalid Numeric Input -> ./simulation_app -i invalid_num\n");
    ProgramConfig cfg2;
    parse_arguments(mock_argc2, mock_argv2, &cfg2);
}

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("   DAY 49: COMMAND LINE ARGUMENTS (ARGC, ARGV)      \n");
    printf("====================================================\n\n");

    /* If run without arguments, run built-in comprehensive simulation */
    if (argc <= 1) {
        printf("[No CLI args supplied via terminal. Running internal simulation...]\n\n");
        run_simulation();
    } else {
        /* Real CLI execution */
        ProgramConfig config;
        int status = parse_arguments(argc, argv, &config);
        if (status == 0) {
            printf("[Parsed Command Line Arguments Successfully]\n");
            printf("  Program Name: %s\n", argv[0]);
            printf("  Verbose     : %s\n", config.verbose ? "true" : "false");
            printf("  Iterations  : %d\n", config.iterations);
            printf("  Output File : %s\n", config.output_file ? config.output_file : "(stdout)");
            printf("  Input File  : %s\n", config.input_file ? config.input_file : "(none)");
        } else if (status == 1) {
            return 1;
        }
    }

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}

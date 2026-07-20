/**
 * ============================================================================
 * Day 44: Contiguous Allocation and Zero Initialization with calloc()
 * File: 44_Calloc_Zero_Init_code.c
 * Standard: ANSI C99
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 44_Calloc_Zero_Init_code.c -o 44_calloc_zero
 *
 * Concepts Demonstrated:
 *   1. Zero-initialization guarantees across primitive types and structures.
 *   2. Frequency histogram analysis utilizing zero-initialized counters.
 *   3. Safe dynamic 2D grid allocation with calloc().
 *   4. Detecting and handling memory allocation failure gracefully.
 *
 * Complexity:
 *   - Histogram Creation: O(N) time where N is string length, O(Alphabet) space.
 *   - Grid Allocation: O(Rows * Cols) time and space.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26

/**
 * build_char_frequency - Counts lowercase ASCII letters using calloc-backed table.
 * @param text: Input string to analyze.
 * @return: Pointer to 26-element array initialized to 0 and populated, or NULL on error.
 */
int *build_char_frequency(const char *text) {
    if (text == NULL) {
        return NULL;
    }

    /* Allocate zero-initialized frequency table */
    int *freq = (int *)calloc(ALPHABET_SIZE, sizeof(int));
    if (freq == NULL) {
        return NULL;
    }

    /* Count character occurrences */
    for (size_t i = 0; text[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (isalpha(ch)) {
            size_t idx = (size_t)(tolower(ch) - 'a');
            if (idx < ALPHABET_SIZE) {
                freq[idx]++;
            }
        }
    }

    return freq;
}

/**
 * create_zero_grid - Allocates a contiguous 2D grid initialized to 0.0.
 * @param rows: Number of rows.
 * @param cols: Number of columns.
 * @return: Contiguous row-pointer matrix, or NULL on failure.
 */
double **create_zero_grid(size_t rows, size_t cols) {
    if (rows == 0 || cols == 0) {
        return NULL;
    }

    /* Allocate array of row pointers */
    double **grid = (double **)calloc(rows, sizeof(double *));
    if (grid == NULL) {
        return NULL;
    }

    /* Allocate each row with calloc ensuring all double elements are 0.0 */
    for (size_t r = 0; r < rows; ++r) {
        grid[r] = (double *)calloc(cols, sizeof(double));
        if (grid[r] == NULL) {
            for (size_t k = 0; k < r; ++k) {
                free(grid[k]);
            }
            free(grid);
            return NULL;
        }
    }

    return grid;
}

/**
 * free_grid - Safely frees the allocated 2D grid.
 */
void free_grid(double **grid, size_t rows) {
    if (grid == NULL) return;
    for (size_t r = 0; r < rows; ++r) {
        free(grid[r]);
    }
    free(grid);
}

int main(void) {
    printf("====================================================\n");
    printf("   DAY 44: CONTIGUOUS ALLOCATION & ZERO-INIT (CALLOC)\n");
    printf("====================================================\n\n");

    /* ------------------------------------------------------------------------
     * Section 1: Verification of Zero-Initialization
     * ------------------------------------------------------------------------ */
    printf("[1] Verifying calloc() Zero-Init Guarantee:\n");
    const size_t test_count = 5;
    int *zero_ints = (int *)calloc(test_count, sizeof(int));

    if (zero_ints != NULL) {
        printf("    Allocated %zu ints at %p:\n    ", test_count, (void *)zero_ints);
        bool all_zeros = true;
        for (size_t i = 0; i < test_count; ++i) {
            printf("[%zu]=%d ", i, zero_ints[i]);
            if (zero_ints[i] != 0) all_zeros = false;
        }
        printf("\n    Verification: %s\n\n", all_zeros ? "PASS (All 0s)" : "FAIL");
        free(zero_ints);
        zero_ints = NULL;
    }

    /* ------------------------------------------------------------------------
     * Section 2: Character Frequency Histogram Application
     * ------------------------------------------------------------------------ */
    printf("[2] Character Frequency Analysis via calloc():\n");
    const char *sample_text = "The quick brown fox jumps over the lazy dog!";
    printf("    Input Text: \"%s\"\n", sample_text);

    int *frequency = build_char_frequency(sample_text);
    if (frequency != NULL) {
        printf("    Non-zero letter counts:\n    ");
        int printed_in_line = 0;
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (frequency[i] > 0) {
                printf("%c: %d  ", 'a' + i, frequency[i]);
                printed_in_line++;
                if (printed_in_line % 8 == 0) printf("\n    ");
            }
        }
        printf("\n\n");
        free(frequency);
        frequency = NULL;
    }

    /* ------------------------------------------------------------------------
     * Section 3: 2D Grid with Floating Point Zero Initialization
     * ------------------------------------------------------------------------ */
    printf("[3] Floating-Point 2D Grid Initialized via calloc():\n");
    const size_t g_rows = 3;
    const size_t g_cols = 4;
    double **grid = create_zero_grid(g_rows, g_cols);

    if (grid != NULL) {
        /* Mutate selected coordinates */
        grid[0][0] = 1.25;
        grid[1][2] = 9.99;
        grid[2][3] = 4.50;

        printf("    Grid (%zux%zu) with initial state + modifications:\n", g_rows, g_cols);
        for (size_t r = 0; r < g_rows; ++r) {
            printf("    Row %zu: ", r);
            for (size_t c = 0; c < g_cols; ++c) {
                printf("%6.2f ", grid[r][c]);
            }
            printf("\n");
        }

        free_grid(grid, g_rows);
        grid = NULL;
    }

    printf("\n[Execution Completed Successfully]\n");
    return 0;
}

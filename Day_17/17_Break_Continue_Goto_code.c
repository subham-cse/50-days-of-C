/**
 * ============================================================================
 * Day 17: Jump Mechanics - Break, Continue, and Goto
 * Description : Production demonstration of jump instructions:
 *               1. Matrix search with break
 *               2. Data sanitation / filtering with continue
 *               3. Linux kernel style multi-resource error unwinding with goto
 * Compilation : gcc -Wall -Wextra -std=c99 17_Break_Continue_Goto_code.c -o 17_Break_Continue_Goto_code
 * Execution   : ./17_Break_Continue_Goto_code
 * 
 * Complexity  : Time: O(R * C) for 2D matrix search
 *               Space: O(1) Stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Searches for target in a 2D matrix, escaping nested loops via goto / break.
 * @param rows Number of rows.
 * @param cols Number of columns.
 * @param matrix Flat pointer to 2D row-major matrix.
 * @param target Search query.
 * @param out_r Pointer to output row index.
 * @param out_c Pointer to output col index.
 * @return True if found, false otherwise.
 */
bool find_matrix_element(size_t rows, size_t cols, const int *matrix, int target, size_t *out_r, size_t *out_c) {
    bool found = false;

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            if (matrix[r * cols + c] == target) {
                *out_r = r;
                *out_c = c;
                found = true;
                goto search_complete; /* Clean exit from nested loop without flags */
            }
        }
    }

search_complete:
    return found;
}

/**
 * @brief Filters and calculates average of valid non-corrupted positive sensor readings.
 * @param readings Array of raw sensor metrics.
 * @param count Total number of samples.
 * @param out_valid_count Pointer to count of accepted samples.
 * @return Average of valid entries.
 */
double process_valid_sensor_telemetry(const int readings[], size_t count, size_t *out_valid_count) {
    double sum = 0.0;
    size_t valid = 0;

    for (size_t i = 0; i < count; ++i) {
        int val = readings[i];

        /* Sentinel value -999 indicates catastrophic sensor hardware drop */
        if (val == -999) {
            printf("  [Sensor Alert] Emergency stop sentinel encountered at idx %zu! Halting scan.\n", i);
            break; /* Terminate processing */
        }

        /* Negative or out-of-bounds readings are transient noise -> skip */
        if (val < 0 || val > 1000) {
            continue; /* Skip noisy data point, advance to next sample */
        }

        sum += val;
        valid++;
    }

    if (out_valid_count != NULL) *out_valid_count = valid;
    return (valid > 0) ? (sum / (double)valid) : 0.0;
}

/**
 * @brief Simulates OS-level multi-resource allocation and Linux kernel-style cleanup.
 * @param simulate_failure_step Step (1, 2, or 3) where allocation failure is simulated.
 * @return 0 on success, negative error code on failure.
 */
int kernel_style_resource_pipeline(int simulate_failure_step) {
    int *buffer_a = NULL;
    int *buffer_b = NULL;
    int *buffer_c = NULL;
    int status = 0;

    printf("  [Alloc 1] Allocating Primary Buffer A... ");
    if (simulate_failure_step == 1) {
        printf("FAILED\n");
        status = -1;
        goto err_buf_a;
    }
    buffer_a = (int *)malloc(1024 * sizeof(int));
    printf("OK\n");

    printf("  [Alloc 2] Allocating Secondary Buffer B... ");
    if (simulate_failure_step == 2) {
        printf("FAILED\n");
        status = -2;
        goto err_buf_b;
    }
    buffer_b = (int *)malloc(2048 * sizeof(int));
    printf("OK\n");

    printf("  [Alloc 3] Allocating Auxiliary Buffer C... ");
    if (simulate_failure_step == 3) {
        printf("FAILED\n");
        status = -3;
        goto err_buf_c;
    }
    buffer_c = (int *)malloc(4096 * sizeof(int));
    printf("OK\n");

    printf("  -> All subsystems successfully initialized. Executing tasks...\n");
    /* Clean, normal teardown path */
    free(buffer_c);
    free(buffer_b);
    free(buffer_a);
    return 0;

/* Error Unwinding Cascades in reverse allocation order */
err_buf_c:
    printf("  [Cleanup] Freeing Buffer B...\n");
    free(buffer_b);
err_buf_b:
    printf("  [Cleanup] Freeing Buffer A...\n");
    free(buffer_a);
err_buf_a:
    printf("  [Cleanup] Handler complete. Returning error code %d\n", status);
    return status;
}

int main(void) {
    printf("===================================================================\n");
    printf("       DAY 17: UNCONDITIONAL JUMPS (BREAK, CONTINUE, GOTO)         \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: Matrix Multi-Loop Breakout via Goto */
    printf("[TEST SUITE 1: 2D Grid Multi-Loop Breakout]\n");
    int grid[3][4] = {
        {10, 20, 30, 40},
        {50, 60, 77, 80},
        {90, 100, 110, 120}
    };
    size_t r = 0, c = 0;
    int target = 77;
    if (find_matrix_element(3, 4, (const int *)grid, target, &r, &c)) {
        printf("Element %d found at Row %zu, Column %zu\n", target, r, c);
    } else {
        printf("Element %d not found.\n", target);
    }

    /* Test Suite 2: Sensor Telemetry Filtering with Continue & Break */
    printf("\n[TEST SUITE 2: Telemetry Noise Filter (Continue & Break)]\n");
    int sensor_stream[] = {250, 260, -5, 270, 1500, 280, -999, 290, 300};
    size_t total_samples = sizeof(sensor_stream) / sizeof(sensor_stream[0]);
    size_t valid_count = 0;

    double avg = process_valid_sensor_telemetry(sensor_stream, total_samples, &valid_count);
    printf("Valid Samples: %zu | Computed Clean Average: %.2f\n", valid_count, avg);

    /* Test Suite 3: Linux Kernel RAII Error Cascade */
    printf("\n[TEST SUITE 3: Kernel Cascaded Error Unwind via Goto]\n");
    printf("Test Scenario: Failure at Subsystem Step 3:\n");
    int ret = kernel_style_resource_pipeline(3);
    printf("Pipeline Return Status: %d\n\n", ret);

    printf("Test Scenario: All Subsystems Normal:\n");
    ret = kernel_style_resource_pipeline(0);
    printf("Pipeline Return Status: %d\n", ret);

    printf("===================================================================\n");
    return 0;
}

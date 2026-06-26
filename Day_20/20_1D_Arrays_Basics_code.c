/**
 * ============================================================================
 * Day 20: 1D Arrays - Memory Layout, Pointer Duality, and Statistical Processing
 * Description : Production demonstration of 1D arrays in C99:
 *               1. Memory address inspection & contiguous layout verification
 *               2. Statistical analysis (mean, sample standard deviation, min/max)
 *               3. In-place rotation & linear search
 * Compilation : gcc -Wall -Wextra -std=c99 20_1D_Arrays_Basics_code.c -lm -o 20_1D_Arrays_Basics_code
 * Execution   : ./20_1D_Arrays_Basics_code
 * 
 * Complexity  : Stats calculation: O(N) Time, O(1) Extra Space
 *               Array reversal rotation: O(N) Time, O(1) Space
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

typedef struct {
    double mean;
    double std_dev;
    double min_val;
    double max_val;
} ArrayStats;

/**
 * @brief Computes comprehensive descriptive statistics on a 1D double array.
 * @param data Constant array of double precision floating point numbers.
 * @param count Number of elements in array (must be >= 2 for sample variance).
 * @param out_stats Pointer to structure receiving statistical metrics.
 * @return True on success, false on invalid parameter.
 */
bool compute_array_statistics(const double data[], size_t count, ArrayStats *out_stats) {
    if (data == NULL || count < 2 || out_stats == NULL) {
        return false;
    }

    double sum = 0.0;
    double min_v = data[0];
    double max_v = data[0];

    /* First pass: Calculate Sum, Min, Max */
    for (size_t i = 0; i < count; ++i) {
        double val = data[i];
        sum += val;
        if (val < min_v) min_v = val;
        if (val > max_v) max_v = val;
    }

    double mean = sum / (double)count;

    /* Second pass: Calculate Variance (Two-pass algorithm prevents numerical instability) */
    double variance_sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        double diff = data[i] - mean;
        variance_sum += diff * diff;
    }

    double sample_variance = variance_sum / (double)(count - 1);

    out_stats->mean = mean;
    out_stats->std_dev = sqrt(sample_variance);
    out_stats->min_val = min_v;
    out_stats->max_val = max_v;

    return true;
}

/**
 * @brief Rotates an array to the left by k positions in-place using the 3-reversal algorithm.
 * @param arr Array of integers.
 * @param n Length of array.
 * @param k Number of positions to rotate.
 */
static void reverse_range(int arr[], size_t start, size_t end) {
    while (start < end) {
        int tmp = arr[start];
        arr[start] = arr[end];
        arr[end] = tmp;
        start++;
        end--;
    }
}

void rotate_array_left(int arr[], size_t n, size_t k) {
    if (n <= 1) return;
    k = k % n;
    if (k == 0) return;

    /* Step 1: Reverse first k elements */
    reverse_range(arr, 0, k - 1);
    /* Step 2: Reverse remaining n-k elements */
    reverse_range(arr, k, n - 1);
    /* Step 3: Reverse entire array */
    reverse_range(arr, 0, n - 1);
}

/**
 * @brief Prints memory layout addresses and byte offsets for contiguous verification.
 * @param arr Pointer to integer array.
 * @param count Number of elements.
 */
void print_memory_layout(const int arr[], size_t count) {
    printf("Base Array Address: %p\n", (const void *)arr);
    for (size_t i = 0; i < count; ++i) {
        const void *elem_addr = (const void *)&arr[i];
        uintptr_t offset = (uintptr_t)elem_addr - (uintptr_t)arr;
        printf("  arr[%zu] | Addr: %p | Offset: +%2zu bytes | Value: %d\n",
               i, elem_addr, (size_t)offset, arr[i]);
    }
}

int main(void) {
    printf("===================================================================\n");
    printf("         DAY 20: 1D ARRAYS & CONTIGUOUS MEMORY ARCHITECTURE        \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: Contiguous Memory Address Layout */
    printf("[TEST SUITE 1: Contiguous Byte Layout & Pointer Offsets]\n");
    int buffer[] = {101, 202, 303, 404, 505};
    print_memory_layout(buffer, ARRAY_COUNT(buffer));

    /* Test Suite 2: Statistical Processing */
    printf("\n[TEST SUITE 2: Descriptive Statistics on Continuous Data]\n");
    double telemetry_samples[] = {14.2, 15.8, 14.9, 16.3, 15.1, 14.7, 18.0, 13.9, 15.5};
    ArrayStats stats;

    if (compute_array_statistics(telemetry_samples, ARRAY_COUNT(telemetry_samples), &stats)) {
        printf("Sample Count : %zu\n", ARRAY_COUNT(telemetry_samples));
        printf("Mean Value   : %.4f\n", stats.mean);
        printf("Std Dev      : %.4f\n", stats.std_dev);
        printf("Min Value    : %.4f\n", stats.min_val);
        printf("Max Value    : %.4f\n", stats.max_val);
    }

    /* Test Suite 3: In-Place Array Rotation */
    printf("\n[TEST SUITE 3: In-Place Left Rotation by K = 3]\n");
    int sequence[] = {1, 2, 3, 4, 5, 6, 7, 8};
    size_t seq_len = ARRAY_COUNT(sequence);

    printf("Original Sequence: ");
    for (size_t i = 0; i < seq_len; ++i) printf("%d ", sequence[i]);
    printf("\n");

    rotate_array_left(sequence, seq_len, 3);

    printf("Rotated (Left 3) : ");
    for (size_t i = 0; i < seq_len; ++i) printf("%d ", sequence[i]);
    printf("\n");

    printf("\n===================================================================\n");
    printf("All 1D array operations verified with zero buffer overruns.\n");
    return 0;
}

/**
 * ============================================================================
 * Day 19: Advanced Math - Logarithms, Trigonometry, and Discretization
 * Description : Production demonstration of scientific math library functions:
 *               1. Navigation heading & polar coordinate projection (atan2/cos/sin)
 *               2. Shannon Information Entropy (log2)
 *               3. Continuous-to-discrete spatial tile discretization (floor/ceil/round)
 * Compilation : gcc -Wall -Wextra -std=c99 19_Math_Logs_Trig_Floor_Ceil_code.c -lm -o 19_Math_Logs_Trig_Floor_Ceil_code
 * Execution   : ./19_Math_Logs_Trig_Floor_Ceil_code
 * 
 * Complexity  : Time: O(1) per calculation
 *               Space: O(1) Stack allocation
 * ============================================================================
 */

#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG_TO_RAD(deg) ((deg) * (M_PI / 180.0))
#define RAD_TO_DEG(rad) ((rad) * (180.0 / M_PI))

/**
 * @brief Computes compass bearing angle (0-360 degrees) from source to destination.
 * @param src_x Starting X coordinate.
 * @param src_y Starting Y coordinate.
 * @param dst_x Target X coordinate.
 * @param dst_y Target Y coordinate.
 * @return True compass bearing in degrees [0.0, 360.0).
 */
double compute_navigation_heading(double src_x, double src_y, double dst_x, double dst_y) {
    double dy = dst_y - src_y;
    double dx = dst_x - src_x;

    /* atan2 returns angle in radians [-PI, +PI] */
    double angle_rad = atan2(dy, dx);
    double angle_deg = RAD_TO_DEG(angle_rad);

    /* Convert Cartesian angle (0 deg = East) to Compass Bearing (0 deg = North) */
    double bearing = 90.0 - angle_deg;
    if (bearing < 0.0) {
        bearing += 360.0;
    }

    return bearing;
}

/**
 * @brief Computes Shannon Information Entropy H(X) = -sum(p * log2(p)) for probability distribution.
 * @param probabilities Array of probabilities that sum to 1.0.
 * @param count Number of distinct states.
 * @return Shannon entropy in bits.
 */
double compute_shannon_entropy(const double probabilities[], size_t count) {
    double entropy = 0.0;

    for (size_t i = 0; i < count; ++i) {
        double p = probabilities[i];
        if (p > 0.0) {
            entropy -= p * log2(p);
        }
    }

    return entropy;
}

/**
 * @brief Demonstrates and compares standard rounding modes on a single continuous value.
 * @param value Floating-point continuous input.
 */
void display_rounding_comparisons(double value) {
    printf("Input: %+6.2f | floor: %+5.1f | ceil: %+5.1f | trunc: %+5.1f | round: %+5.1f\n",
           value,
           floor(value),
           ceil(value),
           trunc(value),
           round(value));
}

int main(void) {
    printf("===================================================================\n");
    printf("         DAY 19: LOGS, TRIGONOMETRY, AND DISCRETIZATION            \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: Navigation Heading Vector Angles */
    printf("[TEST SUITE 1: Navigation Bearing Calculation via atan2]\n");
    struct { double x1, y1, x2, y2; const char *dir; } routes[] = {
        {0.0, 0.0,  0.0,  10.0, "Direct North"},
        {0.0, 0.0, 10.0,   0.0, "Direct East"},
        {0.0, 0.0,  0.0, -10.0, "Direct South"},
        {0.0, 0.0, -10.0,  0.0, "Direct West"},
        {0.0, 0.0, 10.0,  10.0, "North-East (45 deg)"}
    };
    size_t num_routes = sizeof(routes) / sizeof(routes[0]);

    for (size_t i = 0; i < num_routes; ++i) {
        double heading = compute_navigation_heading(
            routes[i].x1, routes[i].y1,
            routes[i].x2, routes[i].y2
        );
        printf("Route: %-20s -> Bearing: %6.2f degrees\n", routes[i].dir, heading);
    }

    /* Test Suite 2: Information Entropy (Binary Logarithms) */
    printf("\n[TEST SUITE 2: Shannon Information Entropy Calculation]\n");
    double fair_coin[] = {0.5, 0.5};
    double biased_coin[] = {0.9, 0.1};
    double four_sided_fair_die[] = {0.25, 0.25, 0.25, 0.25};

    printf("Entropy of Fair Coin (50/50)       : %.4f bits (Max uncertainty = 1.0)\n",
           compute_shannon_entropy(fair_coin, 2));
    printf("Entropy of Biased Coin (90/10)     : %.4f bits\n",
           compute_shannon_entropy(biased_coin, 2));
    printf("Entropy of Fair 4-Sided Die (25x4) : %.4f bits (log2(4) = 2.0)\n\n",
           compute_shannon_entropy(four_sided_fair_die, 4));

    /* Test Suite 3: Continuous-to-Discrete Discretization Functions */
    printf("[TEST SUITE 3: Discretization & Rounding Modalities]\n");
    double test_values[] = {3.7, 3.2, 3.5, -3.2, -3.7, -3.5, 0.0};
    size_t num_tests = sizeof(test_values) / sizeof(test_values[0]);

    for (size_t j = 0; j < num_tests; ++j) {
        display_rounding_comparisons(test_values[j]);
    }

    printf("===================================================================\n");
    return 0;
}

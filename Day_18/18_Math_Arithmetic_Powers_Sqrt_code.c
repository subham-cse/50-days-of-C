/**
 * ============================================================================
 * Day 18: Math Arithmetic - Powers, Exponents, and Square Roots
 * Description : Production demonstration of IEEE 754 math functions in C99:
 *               1. Euclidean 3D distance and vector norm calculation
 *               2. Compound interest financial modeling
 *               3. Quadratic formula root solver with discriminant validation
 * Compilation : gcc -Wall -Wextra -std=c99 18_Math_Arithmetic_Powers_Sqrt_code.c -lm -o 18_Math_Arithmetic_Powers_Sqrt_code
 * Execution   : ./18_Math_Arithmetic_Powers_Sqrt_code
 * 
 * Complexity  : Time: O(1) Hardware FPU accelerated operations
 *               Space: O(1) Stack memory
 * ============================================================================
 */

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>

typedef struct {
    double x;
    double y;
    double z;
} Point3D;

/**
 * @brief Computes true Euclidean distance between two 3D spatial points.
 * @param p1 First 3D coordinate.
 * @param p2 Second 3D coordinate.
 * @return Scalar distance.
 */
double compute_3d_distance(Point3D p1, Point3D p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double dz = p2.z - p1.z;

    /* hypot(hypot(dx, dy), dz) eliminates intermediate overflow */
    return hypot(hypot(dx, dy), dz);
}

/**
 * @brief Computes future compounded investment value: A = P * (1 + r/n)^(n*t)
 * @param principal Initial investment amount ($).
 * @param annual_rate Decimal interest rate (e.g., 0.05 for 5%).
 * @param compound_freq Frequency of compounding per year (e.g., 12 for monthly).
 * @param years Duration of investment in years.
 * @return Final accumulated investment balance.
 */
double compute_compound_growth(double principal, double annual_rate, int compound_freq, double years) {
    if (principal <= 0.0 || annual_rate < 0.0 || compound_freq <= 0 || years <= 0.0) {
        return principal;
    }

    double base = 1.0 + (annual_rate / (double)compound_freq);
    double exponent = (double)compound_freq * years;

    return principal * pow(base, exponent);
}

/**
 * @brief Solves quadratic equation a*x^2 + b*x + c = 0 using robust discriminant checks.
 * @param a Coefficient a (must be non-zero).
 * @param b Coefficient b.
 * @param c Coefficient c.
 * @param out_r1 Pointer to store first real root.
 * @param out_r2 Pointer to store second real root.
 * @return Number of real roots (0, 1, or 2). Returns -1 if degenerate (a == 0).
 */
int solve_quadratic_equation(double a, double b, double c, double *out_r1, double *out_r2) {
    if (fabs(a) < 1e-12) {
        /* Degenerate linear equation: b*x + c = 0 */
        if (fabs(b) < 1e-12) return -1; /* No solution or infinite */
        if (out_r1) *out_r1 = -c / b;
        return 1;
    }

    double discriminant = b * b - 4.0 * a * c;

    if (discriminant > 1e-12) {
        /* Two distinct real roots */
        double sqrt_disc = sqrt(discriminant);
        if (out_r1) *out_r1 = (-b + sqrt_disc) / (2.0 * a);
        if (out_r2) *out_r2 = (-b - sqrt_disc) / (2.0 * a);
        return 2;
    } else if (fabs(discriminant) <= 1e-12) {
        /* Single repeated real root */
        if (out_r1) *out_r1 = -b / (2.0 * a);
        if (out_r2) *out_r2 = *out_r1;
        return 1;
    } else {
        /* Complex conjugate roots (no real roots) */
        return 0;
    }
}

int main(void) {
    printf("===================================================================\n");
    printf("         DAY 18: MATH ARITHMETIC, POWERS, & SQUARE ROOTS           \n");
    printf("===================================================================\n\n");

    /* Test Suite 1: 3D Euclidean Distance */
    printf("[TEST SUITE 1: 3D Spatial Geometry Distance]\n");
    Point3D origin = {0.0, 0.0, 0.0};
    Point3D target = {3.0, 4.0, 12.0};
    double dist = compute_3d_distance(origin, target);
    printf("Distance between (0,0,0) and (3,4,12) = %.4f (Expected: 13.0000)\n\n", dist);

    /* Test Suite 2: Compound Interest Calculation */
    printf("[TEST SUITE 2: Financial Investment Growth Simulation]\n");
    double principal = 10000.0; /* $10,000 */
    double rate = 0.07;         /* 7% annual yield */
    int freq = 12;              /* Compounded monthly */
    double durations[] = {1.0, 5.0, 10.0, 20.0, 30.0};
    size_t num_durations = sizeof(durations) / sizeof(durations[0]);

    printf("Initial Principal: $%.2f at %.1f%% annual interest:\n", principal, rate * 100.0);
    for (size_t i = 0; i < num_durations; ++i) {
        double future_val = compute_compound_growth(principal, rate, freq, durations[i]);
        printf("  After %4.1f years: $%12.2f (Yield: +$%10.2f)\n",
               durations[i], future_val, future_val - principal);
    }

    /* Test Suite 3: Quadratic Equation Real Roots Solver */
    printf("\n[TEST SUITE 3: Quadratic Equation Real Root Solver]\n");
    struct { double a, b, c; } equations[] = {
        {1.0, -5.0, 6.0},   /* x^2 - 5x + 6 = 0 -> roots: 3, 2 */
        {1.0, -4.0, 4.0},   /* x^2 - 4x + 4 = 0 -> root: 2 */
        {1.0, 2.0, 5.0}     /* x^2 + 2x + 5 = 0 -> complex roots */
    };
    size_t num_eq = sizeof(equations) / sizeof(equations[0]);

    for (size_t j = 0; j < num_eq; ++j) {
        double r1 = 0.0, r2 = 0.0;
        int count = solve_quadratic_equation(equations[j].a, equations[j].b, equations[j].c, &r1, &r2);

        printf("Equation: (%.1f)x^2 + (%.1f)x + (%.1f) = 0\n  -> ",
               equations[j].a, equations[j].b, equations[j].c);
        if (count == 2) {
            printf("Two Real Roots: x1 = %.4f, x2 = %.4f\n", r1, r2);
        } else if (count == 1) {
            printf("One Repeated Real Root: x = %.4f\n", r1);
        } else if (count == 0) {
            printf("No Real Roots (Complex Conjugates)\n");
        } else {
            printf("Degenerate Equation\n");
        }
    }

    printf("\n===================================================================\n");
    return 0;
}
